#include "cspch.h"
#include "BrightColorExtractingPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {

	void BrightColorExtractingPipeline::Begin(const Shared<Scene>& scene)
	{
		ComputePipeline::Begin(scene);

		auto device = Device::Instance().GetD3DDevice();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, handle, scene->FloatingPointBuffer->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, handle, scene->BrightColorBuffer->GetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2DARRAY),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}
