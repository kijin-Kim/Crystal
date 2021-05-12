#include "cspch.h"
#include "PanoToCubemapPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {


	void PanoToCubemapPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		ComputePipeline::Begin(pipelineInputs);


		auto device = Device::Instance().GetD3DDevice();
		auto& newResourceManager = NewResourceManager::Instance();

		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		auto& scene = level->GetScene();

		auto& resourceManager = ResourceManager::Instance();


		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(
			1, handle,
			newResourceManager.GetTexture("assets/textures/cubemaps/T_Skybox_11_HybridNoise.hdr")->
			                   NewGetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		;
		handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(
			1, handle, scene.CubemapTexture->NewGetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2DARRAY),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
		);
	}
}
