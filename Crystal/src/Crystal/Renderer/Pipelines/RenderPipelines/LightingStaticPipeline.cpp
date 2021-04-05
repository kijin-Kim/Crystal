#include "cspch.h"
#include "LightingStaticPipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

	void LightingStaticPipeline::OnCreate()
	{
		RenderPipeline::OnCreate();


		m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(PerFrameData));
		
		const int maxObjectCount = 80;
		const int maxMaterialCount = 5;

		m_PerDrawConstantBufferLists.resize(maxObjectCount);
		for (int i = 0; i < maxObjectCount; i++)
		{
			m_PerObjectConstantBuffers.push_back(std::make_unique<ConstantBuffer>((int)sizeof(PerObjectData)));
			for (int j = 0; j < maxMaterialCount; j++)
			{
				m_PerDrawConstantBufferLists[i][j]
					= std::make_unique<ConstantBuffer>((int)sizeof(PerObjectData));
			}
		}

		
	}

	void LightingStaticPipeline::PrepareRecord(const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::PrepareRecord(pipelineInputs);

		auto& renderer = Renderer::Instance();
		auto device = renderer.GetDevice();

		LightingPipelineInputs* lightPipelineInputs = (LightingPipelineInputs*)pipelineInputs;

		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(renderer.GetCamera()->GetViewProjection());
		auto camPos = renderer.GetCamera()->GetWorldPosition();
		m_PerFrameData.CameraPositionInWorld = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f);
		m_PerFrameData.LightPositionInWorld[0] = DirectX::XMFLOAT4(20000.0f, 20000.0f, 0.0f, 0.0f);
		m_PerFrameData.LightPositionInWorld[1] = DirectX::XMFLOAT4(-20000.0f, 20000.0f, 0.0f, 0.0f);
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);



		D3D12_CPU_DESCRIPTOR_HANDLE irradianceTextureHandle = lightPipelineInputs->IrradiancemapTexture->GetShaderResourceView(); // Per Frame

		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetCPUDescriptorHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/
		for (int i = 0; i < m_Components.size(); i++) // PerObject
		{
			auto meshComponent = m_Components[i].lock();
			if(!meshComponent)
				continue;

			PerObjectData staticMeshPerObjectData = {};

			staticMeshPerObjectData.World = Matrix4x4::Transpose(meshComponent->GetWorldTransform());
			m_PerObjectConstantBuffers[i]->SetData((void*)&staticMeshPerObjectData);

			D3D12_CPU_DESCRIPTOR_HANDLE perObjectConstantBufferHandle = m_PerObjectConstantBuffers[i]->GetCPUDescriptorHandle();
			device->CopyDescriptorsSimple(1, destHeapHandle, perObjectConstantBufferHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto staticMesh = std::static_pointer_cast<StaticMesh>(meshComponent->GetRenderable());

			auto materials = meshComponent->GetMaterials();
			auto shader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));
			for (int j = 0; j < materials.size(); j++)
			{
				if (!IsValidForThisPipeline(materials[j]))
					continue;

				D3D12_CPU_DESCRIPTOR_HANDLE albedoTextureHandle = {}; // PerMaterial
				D3D12_CPU_DESCRIPTOR_HANDLE metallicTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE roughnessTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE normalTextureHandle = {};

				PerDrawData perDrawData = {};

				if (materials[j]->HasTextureInput("AlbedoTexture"))
				{
					perDrawData.bToggleAlbedoTexture = true;
					albedoTextureHandle = materials[j]->GetTextureInput("AlbedoTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("AlbedoColor"))
				{
					perDrawData.bToggleAlbedoTexture = false;
					const DirectX::XMFLOAT4& floatInput = materials[j]->GetFloatInput("AlbedoColor");
					perDrawData.AlbedoColor = { floatInput.x, floatInput.y, floatInput.z };
				}

				if (materials[j]->HasTextureInput("MetallicTexture"))
				{
					perDrawData.bToggleMetallicTexture = true;
					metallicTextureHandle = materials[j]->GetTextureInput("MetallicTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("MetallicConstant"))
				{
					perDrawData.bToggleMetallicTexture = false;
					perDrawData.MetallicConstant = materials[j]->GetFloatInput("MetallicConstant").x;
				}

				if (materials[j]->HasTextureInput("RoughnessTexture"))
				{
					perDrawData.bToggleRoughnessTexture = true;
					roughnessTextureHandle = materials[j]->GetTextureInput("RoughnessTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("RoughnessConstant"))
				{
					perDrawData.bToggleRoughnessTexture = false;
					perDrawData.RoughnessConstant = materials[j]->GetFloatInput("RoughnessConstant").x;
				}

				if (materials[j]->HasTextureInput("NormalTexture"))
				{
					perDrawData.bToggleNormalTexture = true;
					normalTextureHandle = materials[j]->GetTextureInput("NormalTexture")->GetShaderResourceView();
				}
				else
					perDrawData.bToggleNormalTexture = false;

				perDrawData.bToggleIrradianceTexture = true;

				m_PerDrawConstantBufferLists[i][j]->SetData((void*)&perDrawData);
				device->CopyDescriptorsSimple(1, destHeapHandle, m_PerDrawConstantBufferLists[i][j]->GetCPUDescriptorHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (albedoTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, albedoTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (metallicTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, metallicTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (roughnessTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, roughnessTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (normalTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, normalTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
		}

		
	}
}