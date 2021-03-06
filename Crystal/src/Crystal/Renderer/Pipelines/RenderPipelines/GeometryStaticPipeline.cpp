#include "cspch.h"
#include "GeometryStaticPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {
	void GeometryStaticPipeline::Begin()
	{
		RenderPipeline::Begin();

		


		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData));

		auto device = Device::Instance().GetD3DDevice();

		auto& scene = GetScene();


		PerFrameData perFrameData = {};

		perFrameData.ViewProjection = Matrix4x4::Transpose(scene->Cameras[0].lock()->GetViewProjection());

		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));


		D3D12_CPU_DESCRIPTOR_HANDLE irradianceTextureHandle = scene->IrradianceTexture->GetShaderResourceView(
			D3D12_SRV_DIMENSION_TEXTURE2D); // Per Frame

		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetConstantBufferView(),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle,
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		int materialCount = 0;
		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/

		for (int i = 0; i < scene->StaticMeshes.size(); i++) // PerObject
		{
			auto meshComponent = scene->StaticMeshes[i].lock();
			if (!meshComponent)
				continue;

			auto staticMesh = Cast<StaticMesh>(meshComponent->GetRenderable()).get();


			PerInstanceData perInstanceData = {};

			perInstanceData.World = meshComponent->GetWorldTransform();
			auto& materials = meshComponent->GetMaterials();


			for (auto& mat : materials)
			{
				auto matRow = mat.get();

				perInstanceData.AlbedoColor = matRow->AlbedoColor;
				perInstanceData.EmissiveColor = matRow->EmissiveColor;
				perInstanceData.RoughnessConstant = matRow->RoughnessConstant;
				perInstanceData.MetallicConstant = matRow->MetallicConstant;

				perInstanceData.bToggleAlbedoTexture = matRow->AlbedoTexture ? true : false;
				perInstanceData.bToggleMetallicTexture = matRow->MetallicTexture ? true : false;
				perInstanceData.bToggleRoughnessTexture = matRow->RoughnessTexture ? true : false;
				perInstanceData.bToggleNormalTexture = matRow->NormalTexture ? true : false;
				perInstanceData.bToggleEmissivetexture = matRow->EmissiveTexture ? true : false;

				perInstanceData.bToggleIrradianceTexture = true; // TEMP

				auto it = std::find_if(m_InstanceBatches[staticMesh].MaterialLookup.begin(),
				                       m_InstanceBatches[staticMesh].MaterialLookup.end(), [&mat](Material* other)
				                       {
					                       return mat->UsingSameTextures(other);
				                       });

				bool bFindMaterial = it != m_InstanceBatches[staticMesh].MaterialLookup.end();
				if (bFindMaterial)
				{
					perInstanceData.MaterialIndex = it - m_InstanceBatches[staticMesh].MaterialLookup.begin();
					continue;
				}


				for (int j = 0; j < m_InstanceBatches[staticMesh].MaterialLookup.size(); j++)
				{
					if (m_InstanceBatches[staticMesh].MaterialLookup[j])
						continue;

					perInstanceData.MaterialIndex = j;
					m_InstanceBatches[staticMesh].MaterialLookup[j] = matRow;
					break;
				}


				if (m_InstanceBatches[staticMesh].DescriptorOffset == -1)
				{
					m_InstanceBatches[staticMesh].DescriptorOffset =
						device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
						* (2 + 50 * materialCount);
				}


				auto cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				cpuHandle.ptr += m_InstanceBatches[staticMesh].DescriptorOffset;
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* (5 * perInstanceData.MaterialIndex);

				if (perInstanceData.bToggleAlbedoTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->AlbedoTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleMetallicTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->MetallicTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleRoughnessTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->RoughnessTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleNormalTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->NormalTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleEmissivetexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->EmissiveTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


				materialCount++;
			}
			m_InstanceBatches[staticMesh].PerInstanceDatas.push_back(perInstanceData);
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
			                                                 0, sizeof(PerInstanceData) * instanceBatches.
			                                                 PerInstanceDatas.size());
		}
	}

	void GeometryStaticPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto device = Device::Instance().GetD3DDevice();
		auto rootSignature = m_Shader->GetRootSignature();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.GetData());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		int rootParameterIndex = 0;

		if (rootSignature.HasPerFrameParameter())
		{
			commandList->
				SetGraphicsRootDescriptorTable(rootSignature.GetPerFrameParameterIndex(), descriptorHeapHandle);
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

	void GeometryStaticPipeline::End()
	{
		m_InstanceBatches.clear();
	}


}
