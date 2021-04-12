#include "cspch.h"
#include "LightingStaticPipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

	void LightingStaticPipeline::PrepareRecord(const PipelineInputs* const pipelineInputs)
	{
		LightPipeline::PrepareRecord(pipelineInputs);


		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData), sizeof(PerDrawData), 5);

		auto& renderer = Renderer::Instance();
		auto device = renderer.GetDevice();

		LightingPipelineInputs* lightPipelineInputs = (LightingPipelineInputs*)pipelineInputs;


		PerFrameData perFrameData = {};

		perFrameData.ViewProjection = Matrix4x4::Transpose(renderer.GetCamera()->GetViewProjection());
		perFrameData.CameraPositionInWorld = renderer.GetCamera()->GetWorldPosition();


		const int maxLightCount = 20;
		int lightCount = 0;
		for (const auto& weak : m_LocalLightComponents)
		{
			if (lightCount >= maxLightCount)
				break;

			auto localLightComponent = weak.lock();
			if (!localLightComponent)
				continue;

			auto lightPosition =

				perFrameData.Lights[lightCount].WorldPosition = localLightComponent->GetWorldPosition();

			perFrameData.Lights[lightCount].Color = localLightComponent->GetLightColor();
			perFrameData.Lights[lightCount].Intensity = localLightComponent->GetLightIntensity();


			lightCount++;
		}

		perFrameData.LightCount = lightCount;

		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));



		D3D12_CPU_DESCRIPTOR_HANDLE irradianceTextureHandle = lightPipelineInputs->IrradiancemapTexture->GetShaderResourceView(); // Per Frame

		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		


		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/
		for (int i = 0; i < m_Components.size(); i++) // PerObject
		{
			auto meshComponent = m_Components[i].lock();
			if(!meshComponent)
				continue;

			auto staticMesh = Cast<StaticMesh>(meshComponent->GetRenderable());

			PerInstanceData perInstanceData = {};

			perInstanceData.World = meshComponent->GetWorldTransform();
			auto& materials = meshComponent->GetMaterials();

			
			for (auto& mat : materials)
			{

				auto matRow = (LightingStaticPipeline::Material*)mat.get();
				
				perInstanceData.AlbedoColor = matRow->AlbedoColor;
				perInstanceData.EmissiveColor = matRow->EmissiveColor;
				perInstanceData.RoughnessConstant = matRow->RoughnessConstant;
				perInstanceData.MetallicConstant = matRow->MetallicConstant;

				perInstanceData.bToggleAlbedoTexture = matRow->AlbedoTexture.lock() ? true : false;
				perInstanceData.bToggleMetallicTexture = matRow->MetallicTexture.lock() ? true : false;
				perInstanceData.bToggleRoughnessTexture = matRow->RoughnessTexture.lock() ? true : false;
				perInstanceData.bToggleNormalTexture = matRow->NormalTexture.lock() ? true : false;
				perInstanceData.bToggleEmissivetexture = matRow->EmissiveTexture.lock() ? true : false;

				perInstanceData.bToggleIrradianceTexture = true; // TEMP

				auto it = std::find_if(m_MaterialCache[staticMesh].begin(), m_MaterialCache[staticMesh].end(), [&mat](MaterialBase* other)
					{
						return mat->CanBeInstancedTogether(other);
					});

				bool bFindMaterial = it != m_MaterialCache[staticMesh].end();
				if (bFindMaterial)
				{
					perInstanceData.MaterialIndex = it - m_MaterialCache[staticMesh].begin();
					continue;
				}
				
				for (int j = 0; j < m_MaterialCache[staticMesh].size(); j++)
				{
					if(!m_MaterialCache[staticMesh][j])
						continue;

					perInstanceData.MaterialIndex = i;
					m_MaterialCache[staticMesh][j] = matRow;
					break;
				}

				
			
				if (m_RenderableDescriptorHandleOffset.count(staticMesh) == 0)
				{
					m_RenderableDescriptorHandleOffset[staticMesh] = 
						device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
						* (2 + 50 * m_RenderableDescriptorHandleOffset.size() - 1);;
				}

				auto cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				cpuHandle.ptr += m_RenderableDescriptorHandleOffset[staticMesh];

				if (perInstanceData.bToggleAlbedoTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle, matRow->AlbedoTexture.lock()->GetShaderResourceView(),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleMetallicTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle, matRow->MetallicTexture.lock()->GetShaderResourceView(),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleRoughnessTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle, matRow->RoughnessTexture.lock()->GetShaderResourceView(),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleNormalTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle, matRow->NormalTexture.lock()->GetShaderResourceView(),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleEmissivetexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle, matRow->EmissiveTexture.lock()->GetShaderResourceView(),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	




			}
			m_PerInstanceData[staticMesh].push_back(perInstanceData);
			m_MaterialCache.clear();




		}


		m_MaterialCache.clear();

		for (const auto& pair : m_PerInstanceData)
		{
			auto& renderable = pair.first;
			auto& perInstanceData = pair.second;

			if (m_PerInstanceVertexBuffers.count(renderable) == 0)
			{
				m_PerInstanceVertexBuffers[renderable] = std::make_unique<Buffer>(nullptr, sizeof(PerInstanceData) *
					perInstanceData.size(), perInstanceData.size(), false, true);
			}

			m_PerInstanceVertexBuffers[renderable]->SetData((void*)perInstanceData.data(), 0, sizeof(PerInstanceData) *
				perInstanceData.size());

		}

		m_PerInstanceData.clear();

	}

	void LightingStaticPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{

		Pipeline::Record(commandList);

		auto device = Renderer::Instance().GetDevice();
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


		for (const auto& pair : m_PerInstanceVertexBuffers)
		{
			auto& renderable = pair.first;
			auto& perInstanceVertexBuffer = pair.second;

			/*commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerExecuteParameterIndex(), descriptorHeapHandle);
			descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
				* rootSignature.GetPerExecuteDescriptorCount();*/

			auto handle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += m_RenderableDescriptorHandleOffset[renderable];
			commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerExecuteParameterIndex(), handle);
			/*descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
				* rootSignature.GetPerExecuteDescriptorCount();*/
			

			commandList->IASetVertexBuffers(1, 1, &perInstanceVertexBuffer->GetVertexBufferView());
			
			//여기서부터 Texture2D Array Per Instance
			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				renderable->Render(commandList, j, perInstanceVertexBuffer->GetSize() / sizeof(PerInstanceData));
			}
			
		}

	}

}