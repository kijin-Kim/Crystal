#include "cspch.h"
#include "LinePipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

	void LinePipeline::OnCreate()
	{
		RenderPipeline::OnCreate();


		m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(PerFrameData));

		const int maxObjectCount = 300;
		for (int i = 0; i < maxObjectCount; i++)
			m_PerObjectConstantBuffers.push_back(std::make_unique<ConstantBuffer>((int)sizeof(PerObjectData)));
	}

	void LinePipeline::PrepareRecord(const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::PrepareRecord(pipelineInputs);

		auto& renderer = Renderer::Instance();
		auto device = renderer.GetDevice();


		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(renderer.GetCamera()->GetViewProjection());
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);

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

				m_PerObjectData.World = Matrix4x4::Transpose(collisionComponent->GetPostScaledTransform());
				m_PerObjectData.Color = collisionComponent->GetLineColor();
				m_PerObjectConstantBuffers[i]->SetData((void*)&m_PerObjectData);

				device->CopyDescriptorsSimple(1, cpuHandle, m_PerObjectConstantBuffers[i]->GetCPUDescriptorHandle(),
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}

			
		}

	}
}