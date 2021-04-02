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

	void LinePipeline::PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
		const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::PrepareRecord(commandList, pipelineInputs);

		auto device = Renderer::Instance().GetDevice();

		LinePipelineInputs* linePipelineInputs = (LinePipelineInputs*)pipelineInputs;

		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(linePipelineInputs->Camera->GetViewProjection());
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, cpuHandle, m_PerFrameConstantBuffer->GetCPUDescriptorHandle(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < m_Components.size(); i++)
		{
			auto collisionComponent = Cast<CollisionComponent>(m_Components[i]);
			if(!collisionComponent)
				continue;

			DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Identity();

			m_PerObjectData.World = Matrix4x4::Transpose(collisionComponent->GetPostScaledTransform());
			m_PerObjectData.Color = collisionComponent->GetLineColor();
			m_PerObjectConstantBuffers[i]->SetData((void*)&m_PerObjectData);

			device->CopyDescriptorsSimple(1, cpuHandle, m_PerObjectConstantBuffers[i]->GetCPUDescriptorHandle(),
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		//auto shader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));

		//ID3D12DescriptorHeap* descriptorHeaps[] = { m_DescriptorHeap.Get() };
		//commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		//commandList->SetPipelineState(m_PipelineState.Get());
		//commandList->SetGraphicsRootSignature(shader->GetRootSignature().GetData());
		////commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());

		//D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		//commandList->SetGraphicsRootDescriptorTable(0, gpuHandle);

		//gpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		//for (int i = 0; i < m_Components.size(); i++)
		//{
		//	auto collisionComponent = Cast<CollisionComponent>(m_Components[i]);
		//	if (!collisionComponent)
		//		continue;

		//	commandList->SetGraphicsRootDescriptorTable(1, gpuHandle);
		//	auto renderable = collisionComponent->GetRenderable();
		//	for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
		//		renderable->Render(commandList, j);

		//	gpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		//}



		auto shader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));

		auto rootSignature = shader->GetRootSignature();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.GetData());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		int rootParameterIndex = 0;

		if (rootSignature.HasPerFrameParameter())
		{
			commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerFrameParameterIndex(), descriptorHeapHandle);
			descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
				* rootSignature.GetPerFrameDescriptorCount();

		}

		for (const auto& component : m_Components)
		{
			auto meshComponent = component.lock();
			if (!meshComponent)
				continue;

			if (rootSignature.HasPerObjectParameter())
			{
				commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerObjectParameterIndex(), descriptorHeapHandle); // PerObject
				descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* rootSignature.GetPerObjectDescriptorCount();
			}


			auto renderable = meshComponent->GetRenderable();
			auto materials = meshComponent->GetMaterials();
			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				if (materials[j])
				{
					if (rootSignature.GetPerDrawDescriptorCount())
					{
						commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerDrawParameterIndex(), descriptorHeapHandle); // PerMaterial
						descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
							* rootSignature.GetPerDrawDescriptorCount();
					}

				}
				renderable->Render(commandList, j);
			}
		}
	}
}