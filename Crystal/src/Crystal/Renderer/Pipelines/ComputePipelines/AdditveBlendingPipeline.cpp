#include "cspch.h"
#include "AdditveBlendingPipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

	void AdditiveBlendingPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		ComputePipeline::Begin(pipelineInputs);

		auto input = *(AdditiveBlendingPipelineInputs*)pipelineInputs;

		auto device = Renderer::Instance().GetDevice();
		auto& resourceManager = ResourceManager::Instance();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, handle, resourceManager.GetTexture("BrightColorBuffer").lock()->GetShaderResourceView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, handle, resourceManager.GetTexture("FloatingPointBuffer").lock()->GetUnorderedAccessView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

}



