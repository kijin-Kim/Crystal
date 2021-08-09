#include "cspch.h"
#include "ShadowMapSkeletalPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"

namespace Crystal {

	void ShadowMapSkeletalPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

	

		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsConstants(16, 0);
		rootParameters[1].InitAsConstantBufferView(1);


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr,
		                     D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
		                                 rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");


		

		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};




		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = { inputLayout, _countof(inputLayout) };
		RenderTargetDescription renderTargetDescription(DXGI_FORMAT_D32_FLOAT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::Opaque,
			StateHelper::DepthEnable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);


		pipelineStateDescription.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineState);
	}

	void ShadowMapSkeletalPipeline::Begin()
	{
		RenderPipeline::Begin();


		auto& scene = GetScene();


		for (int i = 0; i < scene->SkeletalMeshes.size(); i++)
		{
			auto component = scene->SkeletalMeshes[i].lock();
			if (!component)
				continue;

			if(component->GetHideInGame())
			{
				continue;
			}


			while (scene->SkeletalMeshes.size() > m_PerObjectConstantBuffers.size())
			{
				auto buffer = std::make_unique<Buffer>(nullptr, sizeof(PerObjectData), 0, true, true);
				m_PerObjectConstantBuffers.push_back(std::move(buffer));
			}


			PerObjectData perObjectData = {};

			auto skeletalMesh = Cast<SkeletalMesh>(component->GetRenderable());

			perObjectData.World = Matrix4x4::Transpose(component->GetWorldTransform());
			auto boneMatrices = component->GetBoneTransforms();
			std::copy(boneMatrices.begin(), boneMatrices.end(), perObjectData.Bones); // TODO : 최적화 매우매우매우매우 비효율적
			m_PerObjectConstantBuffers[i]->SetData(&perObjectData, 0, sizeof(perObjectData));
		}
	}

	void ShadowMapSkeletalPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto& device = Device::Instance();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());


		auto& scene = GetScene();

		if(scene->Lights.empty())
		{
			return;
		}

		if (scene->Lights[0].expired())
		{
			return;
		}

		commandList->SetGraphicsRoot32BitConstants(0, 16, &Matrix4x4::Transpose(scene->Lights[0].lock()->GetLightViewProjection()), 0);

		for (int i = 0; i < scene->SkeletalMeshes.size(); i++)
		{
			auto skeletalMeshComponent = scene->SkeletalMeshes[i].lock();
			if (!skeletalMeshComponent)
				continue;
			if (!skeletalMeshComponent->GetCastShadow())
				continue;

			if(skeletalMeshComponent->GetHideInGame())
			{
				continue;
			}
			
			auto renderable = skeletalMeshComponent->GetRenderable().lock();
			if (!renderable)
				continue;

			commandList->SetGraphicsRootConstantBufferView(1, m_PerObjectConstantBuffers[i]->GetGPUVirtualAddress());

			for (int j = 0; j < renderable->GetVertexBuffers().size(); j++)
			{
				renderable->Render(commandList, j, 1);
			}
		}
	}
}
