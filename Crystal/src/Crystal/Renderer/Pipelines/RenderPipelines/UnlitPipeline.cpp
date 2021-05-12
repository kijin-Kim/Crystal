#include "cspch.h"
#include "UnlitPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"


namespace Crystal {

	void UnlitPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		Pipeline::Begin(pipelineInputs);


		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerInstanceData));

		auto device = Device::Instance().GetD3DDevice();

		UnlitPipelineInputs* lightPipelineInputs = (UnlitPipelineInputs*)pipelineInputs;


		PerFrameData perFrameData = {};

		perFrameData.View = Matrix4x4::Transpose(lightPipelineInputs->Camera->GetView());
		perFrameData.Projection = Matrix4x4::Transpose(lightPipelineInputs->Camera->GetProjection());
		

		m_PerFrameConstantBuffer->SetData(&perFrameData, 0, sizeof(perFrameData));



		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		int materialCount = 0;
		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/
		for (int i = 0; i < m_Components.size(); i++) // PerObject
		{
			auto meshComponent = m_Components[i].lock();
			if (!meshComponent)
				continue;

			auto renderable = meshComponent->GetRenderable().get();



			PerInstanceData perInstanceData = {};

			perInstanceData.World = meshComponent->GetWorldTransform();
			auto& materials = meshComponent->GetMaterials();


			for (auto& mat : materials)
			{

				auto matRow = mat.get();

				
				perInstanceData.EmissiveColor = matRow->EmissiveColor;

				perInstanceData.bToggleEmissivetexture = matRow->EmissiveTexture ? true : false;



				auto it = std::find_if(m_InstanceBatches[renderable].MaterialLookup.begin(),
					m_InstanceBatches[renderable].MaterialLookup.end(), [&mat](NewMaterial* other)
					{
						return mat->UsingSameTextures(other);
					});

				bool bFindMaterial = it != m_InstanceBatches[renderable].MaterialLookup.end();
				if (bFindMaterial)
				{
					perInstanceData.MaterialIndex = it - m_InstanceBatches[renderable].MaterialLookup.begin();
					continue;
				}


				for (int j = 0; j < m_InstanceBatches[renderable].MaterialLookup.size(); j++)
				{
					if (m_InstanceBatches[renderable].MaterialLookup[j])
						continue;

					perInstanceData.MaterialIndex = j;
					m_InstanceBatches[renderable].MaterialLookup[j] = matRow;
					break;
				}



				if (m_InstanceBatches[renderable].DescriptorOffset == -1)
				{
					m_InstanceBatches[renderable].DescriptorOffset =
						device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
						* (2 + 10 * materialCount);
				}




				auto cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				cpuHandle.ptr += m_InstanceBatches[renderable].DescriptorOffset;
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* (1 * perInstanceData.MaterialIndex);


				if (perInstanceData.bToggleEmissivetexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle, matRow->EmissiveTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


				materialCount++;
			}
			m_InstanceBatches[renderable].PerInstanceDatas.push_back(perInstanceData);
		}


		for (auto& pair : m_InstanceBatches)
		{
			auto& instanceBatches = pair.second;

			if (!instanceBatches.PerInstanceVertexBuffer)
			{
				instanceBatches.PerInstanceVertexBuffer = std::make_unique<Buffer>(nullptr, sizeof(PerInstanceData) *
					instanceBatches.PerInstanceDatas.size(), instanceBatches.PerInstanceDatas.size(), false, true);
			}

			instanceBatches.PerInstanceVertexBuffer->SetData(instanceBatches.PerInstanceDatas.data(),
				0, sizeof(PerInstanceData) * instanceBatches.PerInstanceDatas.size());

		}

	}

	void UnlitPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{

		Pipeline::Record(commandList);

		auto device = Device::Instance().GetD3DDevice();
		auto rootSignature = m_Shader->GetRootSignature();


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


		for (const auto& pair : m_InstanceBatches)
		{
			auto& renderable = pair.first;
			auto& instanceBatch = pair.second;
			auto& perInstanceVertexBuffer = instanceBatch.PerInstanceVertexBuffer;

			auto handle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += instanceBatch.DescriptorOffset;
			commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerExecuteParameterIndex(), handle);


			commandList->IASetVertexBuffers(1, 1, &perInstanceVertexBuffer->GetVertexBufferView());

			if (!renderable)
			{
				continue;
			}

			//여기서부터 Texture2D Array Per Instance
			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				renderable->Render(commandList, j, perInstanceVertexBuffer->GetSize() / sizeof(PerInstanceData));
			}

		}
	}

	void UnlitPipeline::End()
	{
		m_InstanceBatches.clear();
	}
	
}
