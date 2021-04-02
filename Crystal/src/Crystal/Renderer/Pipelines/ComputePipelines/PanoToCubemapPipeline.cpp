#include "cspch.h"
#include "PanoToCubemapPipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

	 void PanoToCubemapPipeline::OnCreate()
	 {
		 ComputePipeline::OnCreate();

		 auto device = Renderer::Instance().GetDevice();

		 D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		 descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		 descriptorHeapDesc.NumDescriptors = 2;
		 descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		 HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		 CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");

		 CD3DX12_DESCRIPTOR_RANGE1 descHeapRange[2] = {};
		 descHeapRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		 descHeapRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

		 CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
		 rootParameters[0].InitAsDescriptorTable(_countof(descHeapRange), descHeapRange);

		 CD3DX12_STATIC_SAMPLER_DESC computeStaticSampler[1] = {};
		 computeStaticSampler[0].Init(0);

		 CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSig(_countof(rootParameters), rootParameters, _countof(computeStaticSampler), computeStaticSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		 Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		 Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		 hr = D3D12SerializeVersionedRootSignature(&computeRootSig, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		 CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		 hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		 CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");

		 auto& resourceManager = ResourceManager::Instance();

		 auto shader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));
		 auto& shaderDatablobs = shader->GetRaw();
		 D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
		 computePipelineStateDesc.pRootSignature = m_RootSignature.Get();
		 computePipelineStateDesc.CS = { shaderDatablobs[ShaderType::Compute]->GetBufferPointer(), shaderDatablobs[ShaderType::Compute]->GetBufferSize() };

		 hr = device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_PipelineState));
		 CS_FATAL(SUCCEEDED(hr), "PipelineState를 생성하는데 실패하였습니다.");
	 }

	 void PanoToCubemapPipeline::PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs)
	{
		ComputePipeline::PrepareRecord(commandList, pipelineInputs);

		PanoToCubemapPipelineInputs* panoToCubemapPipelineInputs = (PanoToCubemapPipelineInputs*)pipelineInputs;

		auto device = Renderer::Instance().GetDevice();

		/*Compute Pipeline Resource를 복사합니다.*/

		D3D12_CPU_DESCRIPTOR_HANDLE panoToCubemapInputHandles[PANO_TO_CUBEMAP_INPUT_COUNT];

		panoToCubemapInputHandles[SourceTexture] = panoToCubemapPipelineInputs->SourceTexture->GetShaderResourceView();
		panoToCubemapInputHandles[DestinationTexture] = panoToCubemapPipelineInputs->DestinationTexture->GetUnorderedAccessView();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (int i = 0; i < PANO_TO_CUBEMAP_INPUT_COUNT; i++)
		{
			device->CopyDescriptorsSimple(1, handle, panoToCubemapInputHandles[i], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetComputeRootSignature(m_RootSignature.Get());
		ID3D12DescriptorHeap* equiToCubeHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(equiToCubeHeaps), equiToCubeHeaps);
		commandList->SetComputeRootDescriptorTable(0, m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		auto destTextureDesc = panoToCubemapPipelineInputs->DestinationTexture->GetResource()->GetDesc();
		m_DestinationTextureWidth = destTextureDesc.Width;
		m_DestinationTextureHeight = destTextureDesc.Height;

		commandList->Dispatch((UINT)(m_DestinationTextureWidth / 32), (UINT)(m_DestinationTextureHeight / 32), 6);
	}
}