#include "cspch.h"
#include "PanoToCubemapPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {


	void PanoToCubemapPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		ComputePipeline::Begin(pipelineInputs);


		auto device = Device::Instance().GetD3DDevice();

		/*Compute Pipeline Resource를 복사합니다.*/

		for (const auto& componentWeak : m_Components)
		{
			auto component = componentWeak.lock();
			if(!component)
				continue;

			auto materials = component->GetMaterialsOld();
			for (const auto& mat : materials)
			{
				if(!IsValidForThisPipeline(mat) 
					|| !mat->HasTextureInput("EquiTexture") || !mat->HasTextureInput("OutputTexture"))
					continue;


				D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				device->CopyDescriptorsSimple(1, handle, mat->GetTextureInput("EquiTexture")->GetShaderResourceView(), 
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				device->CopyDescriptorsSimple(1, handle, mat->GetTextureInput("OutputTexture")->GetUnorderedAccessView(), 
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				break; // Currently Only One Dispatch Call
			}
		}
	}
}