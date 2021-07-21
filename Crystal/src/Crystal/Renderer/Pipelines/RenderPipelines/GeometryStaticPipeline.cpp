#include "cspch.h"
#include "GeometryStaticPipeline.h"


#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Core/Device.h"

namespace Crystal {

	void GeometryStaticPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 20000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		CD3DX12_DESCRIPTOR_RANGE1 perFrameDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0},
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0}
		};

		CD3DX12_DESCRIPTOR_RANGE1 perExecuteDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
		};


		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsDescriptorTable(_countof(perFrameDescriptorRanges), perFrameDescriptorRanges, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[1].InitAsDescriptorTable(_countof(perExecuteDescriptorRanges), perExecuteDescriptorRanges, D3D12_SHADER_VISIBILITY_ALL);


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
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

			{"MATROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},


			{"ALBEDO_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"EMISSIVE_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"ROUGHNESS_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"METALLIC_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"SHOULD_LIT", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"OPACITY", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_ALBEDO_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_METALLIC_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_ROUGHNESS_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_NORMAL_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_IRRADIANCE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_EMISSIVE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
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
	}

	void GeometryStaticPipeline::Begin()
	{
		RenderPipeline::Begin();


		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData));

		auto device = Device::Instance().GetD3DDevice();

		auto& scene = GetScene();


		PerFrameData perFrameData = {};


		perFrameData.ViewProjection = Matrix4x4::Transpose(scene->Cameras[0].lock()->GetViewProjection());


		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));


		D3D12_CPU_DESCRIPTOR_HANDLE irradianceTextureHandle = scene->IrradianceTexture->GetShaderResourceView(
			D3D12_SRV_DIMENSION_TEXTURECUBE); // Per Frame

		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetConstantBufferView(),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle,
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		

		for (int i = 0; i < scene->StaticMeshes.size(); i++) // PerObject
		{
			auto meshComponent = scene->StaticMeshes[i].lock();
			if (!meshComponent)
				continue;

			auto materials = meshComponent->GetMaterials();

			if (materials.empty())
				continue;
			
			if (!(materials[0]->ShadingModel == EShadingModel::SM_Lit && materials[0]->BlendMode == EBlendMode::BM_Opaque))
				continue;
			

			m_InstancedStaticMesh.AddInstance(meshComponent, m_DescriptorHeap, 2);

		}

		m_InstancedStaticMesh.PrepareDynamicVertexBuffer();
	}

	void GeometryStaticPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto& device = Device::Instance();

		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapHandle);
		descriptorHeapHandle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;


		m_InstancedStaticMesh.Render(commandList, m_DescriptorHeap, 1);
	}

	void GeometryStaticPipeline::End()
	{
		m_InstancedStaticMesh.Clear();
	}


}
