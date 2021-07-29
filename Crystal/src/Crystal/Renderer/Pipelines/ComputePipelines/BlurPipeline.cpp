#include "cspch.h"
#include "BlurPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Resources/DescriptorAllocation.h"
#include "Crystal/Renderer/Scene.h"
namespace Crystal {


	void BlurPipeline::Begin()
	{
		ComputePipeline::Begin();

		auto device = Device::Instance().GetD3DDevice();


		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		auto& scene = level->GetScene();


		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(
			1, handle, scene->BrightColorBuffer->GetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2D),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

}
