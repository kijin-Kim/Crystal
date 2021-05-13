#include "cspch.h"
#include "AdditveBlendingPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Resources/DescriptorAllocator.h"

namespace Crystal {

	void AdditiveBlendingPipeline::Begin()
	{
		ComputePipeline::Begin();


		auto device = Device::Instance().GetD3DDevice();
		

		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		auto& scene = level->GetScene();

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, handle, scene.BrightColorBuffer->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, handle, scene.FloatingPointBuffer->GetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2D),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

}



