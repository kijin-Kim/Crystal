#include "cspch.h"
#include "GeometrySkeletalPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"

namespace Crystal {

	void GeometrySkeletalPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 20000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		CD3DX12_DESCRIPTOR_RANGE1 perFrameRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
		};

		CD3DX12_DESCRIPTOR_RANGE1 perDrawRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
		};


		CD3DX12_ROOT_PARAMETER1 rootParameters[5];
		rootParameters[0].InitAsConstantBufferView(0);
		rootParameters[1].InitAsConstantBufferView(1);
		rootParameters[2].InitAsConstantBufferView(2);
		rootParameters[3].InitAsDescriptorTable(_countof(perFrameRanges), perFrameRanges, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[4].InitAsDescriptorTable(_countof(perDrawRanges), perDrawRanges, D3D12_SHADER_VISIBILITY_ALL);


		CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] = {
			CD3DX12_STATIC_SAMPLER_DESC(0)
		};


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(staticSamplers), staticSamplers,
		                     D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
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


		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 6;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[4] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[5] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {inputLayout, _countof(inputLayout)};
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::Opaque,
			StateHelper::DepthEnable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);

		pipelineStateDescription.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineState);


		m_PerFrameConstantBuffer = CreateUnique<Buffer>(nullptr, sizeof(PerFrameData), 0, true, true);
	}

	void GeometrySkeletalPipeline::Begin()
	{
		RenderPipeline::Begin();


		auto& scene = GetScene();

		auto camera = scene->Cameras[0].lock();


		PerFrameData perFrameData = {};

		perFrameData.ViewProjection = Matrix4x4::Transpose(camera->GetViewProjection());
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto device = Device::Instance().GetD3DDevice();

		auto destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();


		auto irradianceTextureHandle = scene->IrradianceTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/

		for (int i = 0; i < scene->SkeletalMeshes.size(); i++)
		{
			auto component = scene->SkeletalMeshes[i].lock();
			if (!component)
				continue;

			if (component->GetHideInGame())
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


			auto& materials = component->GetMaterials();
			for (int j = 0; j < materials.size(); j++)
			{
				while (materials.size() > m_PerDrawConstantBuffers.size())
				{
					auto buffer = std::make_unique<Buffer>(nullptr, sizeof(PerDrawData), 0, true, true);
					m_PerDrawConstantBuffers.push_back(std::move(buffer));
				}

				PerDrawData perDrawData = {};

				D3D12_CPU_DESCRIPTOR_HANDLE albedoTextureHandle = {}; // PerMaterial
				D3D12_CPU_DESCRIPTOR_HANDLE metallicTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE roughnessTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE normalTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE emissiveTextureHandle = {};

				auto albedoTexture = materials[j]->AlbedoTexture.lock();
				auto metallicTexture = materials[j]->MetallicTexture.lock();
				auto roughnessTexture = materials[j]->RoughnessTexture.lock();
				auto normalTexture = materials[j]->NormalTexture.lock();
				auto emissiveTexture = materials[j]->EmissiveTexture.lock();

				if (albedoTexture)
				{
					perDrawData.bToggleAlbedoTexture = true;
					albedoTextureHandle = albedoTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.AlbedoColor = materials[j]->AlbedoColor;
				}

				if (metallicTexture)
				{
					perDrawData.bToggleMetallicTexture = true;
					metallicTextureHandle = materials[j]->MetallicTexture.lock()->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.bToggleMetallicTexture = false;
					perDrawData.MetallicConstant = materials[j]->MetallicConstant;
				}

				if (roughnessTexture)
				{
					perDrawData.bToggleRoughnessTexture = true;
					roughnessTextureHandle = roughnessTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.bToggleRoughnessTexture = false;
					perDrawData.RoughnessConstant = materials[j]->RoughnessConstant;
				}

				if (normalTexture)
				{
					perDrawData.bToggleNormalTexture = true;
					normalTextureHandle = normalTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.bToggleNormalTexture = false;
				}

				if (emissiveTexture)
				{
					perDrawData.bToggleEmissiveTexture = true;
					emissiveTextureHandle = emissiveTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.bToggleEmissiveTexture = false;
				}


				perDrawData.bToggleIrradianceTexture = true;

				m_PerDrawConstantBuffers[i * j]->SetData(&perDrawData, 0, sizeof(perDrawData));

				if (albedoTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, albedoTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (metallicTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, metallicTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (roughnessTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, roughnessTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (normalTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, normalTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (emissiveTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, emissiveTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
		}
	}

	void GeometrySkeletalPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto& device = Device::Instance();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);


		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());

		auto descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(3, descriptorHeapHandle);
		descriptorHeapHandle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		auto& scene = GetScene();

		for (int i = 0; i < scene->SkeletalMeshes.size(); i++)
		{
			auto skeletalMesh = scene->SkeletalMeshes[i].lock();
			if (!skeletalMesh)
				return;

			if (skeletalMesh->GetHideInGame())
			{
				continue;
			}

			auto renderable = skeletalMesh->GetRenderable().lock();
			if (!renderable)
				return;

			commandList->SetGraphicsRootConstantBufferView(1, m_PerObjectConstantBuffers[i]->GetGPUVirtualAddress());

			for (int j = 0; j < renderable->GetVertexBuffers().size(); j++)
			{
				commandList->SetGraphicsRootConstantBufferView(2, m_PerDrawConstantBuffers[i * j]->GetGPUVirtualAddress());
				commandList->SetGraphicsRootDescriptorTable(4, descriptorHeapHandle);
				descriptorHeapHandle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;

				renderable->Render(commandList, j, 1);
			}
		}
	}


}
