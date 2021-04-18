#include "cspch.h"
#include "CubemapPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {


	void CubemapPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::Begin(pipelineInputs);

		PrepareConstantBuffers(sizeof(PerFrameData));

		CubemapPipelineInputs* cubemapPipelineInputs = (CubemapPipelineInputs*)pipelineInputs;


		PerFrameData perFrameData = {};



		

		auto viewProj = cubemapPipelineInputs->Camera->GetViewProjection();
		viewProj._41 = 0.0f; viewProj._42 = 0.0f; viewProj._43 = 0.0f;
		perFrameData.InverseViewProjection = Matrix4x4::Transpose(Matrix4x4::Inverse(viewProj));
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto device = Device::Instance().GetD3DDevice();

		auto descHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (const auto& weak : m_Components)
		{
			auto component = weak.lock();
			if(!component)
				continue;

			const auto& materials = component->GetMaterialsOld();
			for (const auto& mat : materials)
			{
				if (!IsValidForThisPipeline(mat))
					continue;

				device->CopyDescriptorsSimple(1, descHandle,
					m_PerFrameConstantBuffer->GetConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				device->CopyDescriptorsSimple(1, descHandle,
					mat->GetTextureInput("CubemapColorTexture")->GetShaderResourceView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
		}
	}
}