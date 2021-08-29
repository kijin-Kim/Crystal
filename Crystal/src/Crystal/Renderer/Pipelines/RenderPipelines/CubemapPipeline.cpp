#include "cspch.h"
#include "CubemapPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/BasicShapeMeshes.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"

namespace Crystal {


	void CubemapPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 10;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		CD3DX12_DESCRIPTOR_RANGE1 perFrameDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0},
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
		};


		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsDescriptorTable(_countof(perFrameDescriptorRanges), perFrameDescriptorRanges, D3D12_SHADER_VISIBILITY_ALL);


		CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] = {
			CD3DX12_STATIC_SAMPLER_DESC(0),
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
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};


		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {inputLayout, _countof(inputLayout)};
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);


		PipelineStateDescription noCullNonPre(
			inputLayoutDesc,
			StateHelper::Opaque,
			StateHelper::DepthEnable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);

		noCullNonPre.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineState);
	}

	void CubemapPipeline::Begin(const Shared<Scene>& scene)
	{
		RenderPipeline::Begin(scene);


		PerFrameData perFrameData = {};


		auto camera = scene->Cameras[0].lock();

		auto viewProj = camera->GetViewProjection();
		viewProj._41 = 0.0f;
		viewProj._42 = 0.0f;
		viewProj._43 = 0.0f;
		perFrameData.InverseViewProjection = Matrix4x4::Transpose(Matrix4x4::Inverse(viewProj));
		perFrameData.AdditiveColor = scene->SkyboxConfiguration.AdditiveColor;
		perFrameData.RedLevel = scene->SkyboxConfiguration.RedLevel;
		perFrameData.GreenLevel = scene->SkyboxConfiguration.GreenLevel;
		perFrameData.BlueLevel = scene->SkyboxConfiguration.BlueLevel;
		perFrameData.Power = scene->SkyboxConfiguration.Power;
		perFrameData.Saturation = scene->SkyboxConfiguration.Saturation;
		perFrameData.NearStarDesaturation = scene->SkyboxConfiguration.NearStarDesaturation;
		perFrameData.FarStarDesaturation = scene->SkyboxConfiguration.FarStarDesaturation;

		auto perFrameConstantBuffer = BufferManager::Instance().GetConstantBuffer(&perFrameData, sizeof(PerFrameData));


		auto device = Device::Instance().GetD3DDevice();
		auto descHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, descHandle,
		                              perFrameConstantBuffer->AsConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		device->CopyDescriptorsSimple(1, descHandle,
		                              scene->CubemapColorTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, descHandle,
		                              scene->CubemapAlphaTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, descHandle,
		                              scene->CubemapStarFarTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, descHandle,
		                              scene->CubemapStarNearTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void CubemapPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		auto device = Device::Instance().GetD3DDevice();
		auto rootSignature = m_Shader->GetRootSignature();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.GetData());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapHandle);


		auto renderable = GetScene()->PlaneQuad2DMesh;

		for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
		{
			renderable->Render(commandList, j);
		}
	}
}
