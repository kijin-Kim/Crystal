#include "cspch.h"
#include "LightingSkeletalPipeline.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	void LightingSkeletalPipeline::OnCreate()
	{
		LightPipeline::OnCreate();

		m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(PerFrameData));

		const int maxObjectCount = 50;
		const int maxMaterialCount = 5;

		m_PerDrawConstantBufferLists.resize(maxObjectCount);
		for (int i = 0; i < maxObjectCount; i++)
		{
			m_PerObjectConstantBuffers.push_back(std::make_unique<ConstantBuffer>((int)sizeof(PerObjectData)));
			for (int j = 0; j < maxMaterialCount; j++)
			{
				m_PerDrawConstantBufferLists[i][j]
					= std::make_unique<ConstantBuffer>((int)sizeof(PerDrawData));
			}
		}
	}

	void LightingSkeletalPipeline::PrepareRecord(const PipelineInputs* const pipelineInputs)
	{
		LightPipeline::PrepareRecord(pipelineInputs);

		LightingStaticPipeline::LightingPipelineInputs* lightPipelineInputs = (LightingStaticPipeline::LightingPipelineInputs*)pipelineInputs;

		auto& renderer = Renderer::Instance();

		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(renderer.GetCamera()->GetViewProjection());
		auto camPos = renderer.GetCamera()->GetWorldPosition();
		m_PerFrameData.CameraPositionInWorld = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f);
		m_PerFrameData.LightPositionInWorld[0] = DirectX::XMFLOAT4(20000.0f, 20000.0f, 0.0f, 0.0f);
		m_PerFrameData.LightPositionInWorld[1] = DirectX::XMFLOAT4(-20000.0f, 20000.0f, 0.0f, 0.0f);
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);

		auto device = Renderer::Instance().GetDevice();

		auto destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetCPUDescriptorHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		auto irradianceTextureHandle = lightPipelineInputs->IrradiancemapTexture->GetShaderResourceView();
		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		/*���͸����� Shader Visible Descriptor Heap�� �����մϴ�.*/
		for (int i = 0; i < m_Components.size(); i++)
		{
			auto component = m_Components[i].lock();
			if(!component)
				continue;

			PerObjectData perObjectData = {};

			auto skeletalMesh = Cast<SkeletalMesh>(component->GetRenderable());

			perObjectData.World = Matrix4x4::Transpose(component->GetWorldTransform());
			auto boneMatrices = skeletalMesh->GetBoneTransforms();
			std::copy(boneMatrices.begin(), boneMatrices.end(), perObjectData.Bones);			// TODO : ����ȭ �ſ�ſ�ſ�ſ� ��ȿ����
			m_PerObjectConstantBuffers[i]->SetData((void*)&perObjectData);

			D3D12_CPU_DESCRIPTOR_HANDLE perObjectConstantBufferHandle = m_PerObjectConstantBuffers[i]->GetCPUDescriptorHandle();
			device->CopyDescriptorsSimple(1, destHeapHandle, perObjectConstantBufferHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto materials = component->GetMaterials();
			for (int j = 0; j < materials.size(); j++)
			{
				if (!IsValidForThisPipeline(materials[j]))
					continue;

				PerDrawData perDrawData = {};

				D3D12_CPU_DESCRIPTOR_HANDLE albedoTextureHandle = {}; // PerMaterial
				D3D12_CPU_DESCRIPTOR_HANDLE metallicTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE roughnessTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE normalTextureHandle = {};

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
				device->CopyDescriptorsSimple(1, destHeapHandle, m_PerDrawConstantBufferLists[i][j]->GetCPUDescriptorHandle(), 
					D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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



