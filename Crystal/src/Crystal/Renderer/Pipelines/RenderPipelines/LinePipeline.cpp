#include "cspch.h"
#include "LinePipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

	void LinePipeline::PrepareRecord(const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::PrepareRecord(pipelineInputs);

		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData));

		auto& renderer = Renderer::Instance();
		auto device = renderer.GetDevice();

		PerFrameData perFrameData = {};
		perFrameData.ViewProjection = Matrix4x4::Transpose(renderer.GetCamera()->GetViewProjection());
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData);

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, cpuHandle, m_PerFrameConstantBuffer->GetCPUDescriptorHandle(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < m_Components.size(); i++)
		{
			auto component = m_Components[i].lock();
			if(!component)
				continue;

			auto materials = component->GetMaterials();
			for (const auto& mat : materials)
			{
				if(!IsValidForThisPipeline(mat))
					continue;

				auto collisionComponent = Cast<CollisionComponent>(component);

				DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Identity();

				PerObjectData perObjectData = {};

				perObjectData.World = Matrix4x4::Transpose(collisionComponent->GetPostScaledTransform());
				perObjectData.Color = collisionComponent->GetLineColor();
				m_PerObjectConstantBuffers[i]->SetData((void*)&perObjectData);

				device->CopyDescriptorsSimple(1, cpuHandle, m_PerObjectConstantBuffers[i]->GetCPUDescriptorHandle(),
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}

			
		}

	}
}