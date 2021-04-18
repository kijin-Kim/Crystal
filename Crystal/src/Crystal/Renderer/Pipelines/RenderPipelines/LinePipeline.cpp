#include "cspch.h"
#include "LinePipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

	void LinePipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::Begin(pipelineInputs);

		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData));

		auto device = Device::Instance().GetD3DDevice();

		auto inputs = (RenderPipelineInputs*)pipelineInputs;

		PerFrameData perFrameData = {};
		perFrameData.ViewProjection = Matrix4x4::Transpose(inputs->Camera->GetViewProjection());
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, cpuHandle, m_PerFrameConstantBuffer->GetConstantBufferView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < m_Components.size(); i++)
		{
			auto component = m_Components[i].lock();
			if(!component)
				continue;

			auto materials = component->GetMaterialsOld();
			for (const auto& mat : materials)
			{
				if(!IsValidForThisPipeline(mat))
					continue;

				auto collisionComponent = Cast<CollisionComponent>(component);

				DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Identity();

				PerObjectData perObjectData = {};

				perObjectData.World = Matrix4x4::Transpose(collisionComponent->GetPostScaledTransform());
				perObjectData.Color = collisionComponent->GetLineColor();
				m_PerObjectConstantBuffers[i]->SetData((void*)&perObjectData, 0, sizeof(perObjectData));

				device->CopyDescriptorsSimple(1, cpuHandle, m_PerObjectConstantBuffers[i]->GetConstantBufferView(),
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}

			
		}

	}
}