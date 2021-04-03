#include "cspch.h"
#include "CubemapPipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ConstantBuffer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

	void CubemapPipeline::OnCreate()
	{
		RenderPipeline::OnCreate();

		m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(m_PerFrameData));
	}

	void CubemapPipeline::PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::PrepareRecord(commandList, pipelineInputs);

		CubemapPipelineInputs* cubemapPipelineInputs = (CubemapPipelineInputs*)pipelineInputs;

		auto viewProj = cubemapPipelineInputs->Camera->GetViewProjection();
		viewProj._41 = 0.0f; viewProj._42 = 0.0f; viewProj._43 = 0.0f;
		m_PerFrameData.InverseViewProjection = Matrix4x4::Transpose(Matrix4x4::Inverse(viewProj));
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);

		auto device = Renderer::Instance().GetDevice();

		auto descHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (const auto& weak : m_Components)
		{
			auto component = weak.lock();
			if(!component)
				continue;

			const auto& materials = component->GetMaterials();
			for (const auto& mat : materials)
			{
				if (!IsValidForThisPipeline(mat))
					continue;

				device->CopyDescriptorsSimple(1, descHandle,
					m_PerFrameConstantBuffer->GetCPUDescriptorHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				device->CopyDescriptorsSimple(1, descHandle,
					mat->GetTextureInput("CubemapTexture")->GetShaderResourceView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
		}
	}
}