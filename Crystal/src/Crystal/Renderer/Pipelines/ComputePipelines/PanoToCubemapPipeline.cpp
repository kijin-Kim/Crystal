#include "cspch.h"
#include "PanoToCubemapPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {


	void PanoToCubemapPipeline::Begin()
	{
		ComputePipeline::Begin();


		auto device = Device::Instance().GetD3DDevice();
		auto& resourceManager = ResourceManager::Instance();

		auto& scene = GetScene();


		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, handle, scene->PanoramaCubeColorTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, handle, scene->CubemapColorTexture->GetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2DARRAY),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		device->CopyDescriptorsSimple(1, handle, scene->PanoramaCubeAlphaTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, handle, scene->CubemapAlphaTexture->GetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2DARRAY),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		device->CopyDescriptorsSimple(1, handle, scene->PanoramaStarFarTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, handle, scene->CubemapStarFarTexture->GetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2DARRAY),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		device->CopyDescriptorsSimple(1, handle, scene->PanoramaStarNearTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, handle, scene->CubemapStarNearTexture->GetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2DARRAY),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void PanoToCubemapPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto& device = Device::Instance();

		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetComputeRootSignature(m_Shader->GetRootSignature().GetData());
		ID3D12DescriptorHeap* irradianceSamplingHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(irradianceSamplingHeaps), irradianceSamplingHeaps);


		auto descHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		auto dispatchThreadGroupCounts = m_Shader->GetDispatchThreadGroupCounts();

		commandList->SetComputeRootDescriptorTable(0, descHandle);
		commandList->Dispatch(dispatchThreadGroupCounts.x, dispatchThreadGroupCounts.y, dispatchThreadGroupCounts.z);
		descHandle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;

		commandList->SetComputeRootDescriptorTable(0, descHandle);
		commandList->Dispatch(dispatchThreadGroupCounts.x, dispatchThreadGroupCounts.y, dispatchThreadGroupCounts.z);
		descHandle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;

		commandList->SetComputeRootDescriptorTable(0, descHandle);
		commandList->Dispatch(dispatchThreadGroupCounts.x, dispatchThreadGroupCounts.y, dispatchThreadGroupCounts.z);
		descHandle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;

		commandList->SetComputeRootDescriptorTable(0, descHandle);
		commandList->Dispatch(dispatchThreadGroupCounts.x, dispatchThreadGroupCounts.y, dispatchThreadGroupCounts.z);
		descHandle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2;
	}


}
