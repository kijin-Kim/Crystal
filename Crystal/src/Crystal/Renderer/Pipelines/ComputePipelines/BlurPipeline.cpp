#include "cspch.h"
#include "BlurPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {



	void BlurPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		ComputePipeline::Begin(pipelineInputs);

		auto device = Device::Instance().GetD3DDevice();
		auto& resourceManager = ResourceManager::Instance();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, handle, resourceManager.GetTexture("BrightColorBuffer").lock()->GetUnorderedAccessView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

}