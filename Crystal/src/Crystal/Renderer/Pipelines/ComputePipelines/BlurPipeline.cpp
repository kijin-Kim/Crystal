#include "cspch.h"
#include "BlurPipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {



	void BlurPipeline::PrepareRecord(const PipelineInputs* const pipelineInputs)
	{
		ComputePipeline::PrepareRecord(pipelineInputs);

		auto device = Renderer::Instance().GetDevice();
		auto& resourceManager = ResourceManager::Instance();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, handle, resourceManager.GetTexture("BrightColorBuffer").lock()->GetUnorderedAccessView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

}