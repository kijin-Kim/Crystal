#include "cspch.h"
#include "LightingSkeletalPipeline.h"

#include "LightingStaticPipeline.h"
#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/DescriptorAllocator.h"

namespace Crystal {

	void LightingSkeletalPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		LightPipeline::Begin(pipelineInputs);


		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData), sizeof(PerDrawData), 5);

		LightingStaticPipeline::LightingPipelineInputs* lightPipelineInputs = (LightingStaticPipeline::LightingPipelineInputs*)pipelineInputs;


		PerFrameData perFrameData = {};

		perFrameData.ViewProjection = Matrix4x4::Transpose(lightPipelineInputs->Camera->GetViewProjection());
		auto camPos = lightPipelineInputs->Camera->GetWorldPosition();
		perFrameData.CameraPositionInWorld = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f);
		perFrameData.LightPositionInWorld[0] = DirectX::XMFLOAT4(20000.0f, 20000.0f, 0.0f, 0.0f);
		perFrameData.LightPositionInWorld[1] = DirectX::XMFLOAT4(-20000.0f, 20000.0f, 0.0f, 0.0f);
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto device = Device::Instance().GetD3DDevice();

		auto destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		auto& scene = level->GetScene();

		

		
		auto irradianceTextureHandle = scene.IrradianceTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/
		for (int i = 0; i < m_Components.size(); i++)
		{
			auto component = m_Components[i].lock();
			if(!component)
				continue;

			PerObjectData perObjectData = {};

			auto skeletalMesh = Cast<SkeletalMesh>(component->GetRenderable());

			perObjectData.World = Matrix4x4::Transpose(component->GetWorldTransform());
			auto boneMatrices = skeletalMesh->GetBoneTransforms();
			std::copy(boneMatrices.begin(), boneMatrices.end(), perObjectData.Bones);			// TODO : 최적화 매우매우매우매우 비효율적
			m_PerObjectConstantBuffers[i]->SetData((void*)&perObjectData, 0, sizeof(perObjectData));

			D3D12_CPU_DESCRIPTOR_HANDLE perObjectConstantBufferHandle = m_PerObjectConstantBuffers[i]->GetConstantBufferView();
			device->CopyDescriptorsSimple(1, destHeapHandle, perObjectConstantBufferHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto& materials = component->GetMaterials();
			for (int j = 0; j < materials.size(); j++)
			{
				
				PerDrawData perDrawData = {};

				D3D12_CPU_DESCRIPTOR_HANDLE albedoTextureHandle = {}; // PerMaterial
				D3D12_CPU_DESCRIPTOR_HANDLE metallicTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE roughnessTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE normalTextureHandle = {};

				{
					perDrawData.bToggleAlbedoTexture = true;
					albedoTextureHandle = materials[j]->AlbedoTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				{
					perDrawData.AlbedoColor = materials[j]->AlbedoColor;
				}

				{
					/*perDrawData.bToggleMetallicTexture = true;
					metallicTextureHandle = materials[j]->MetallicTexture.lock()->GetShaderResourceView();*/
				}
				{
					//perDrawData.bToggleMetallicTexture = false;
					perDrawData.MetallicConstant = materials[j]->MetallicConstant;
				}

				{
					perDrawData.bToggleRoughnessTexture = true;
					roughnessTextureHandle = materials[j]->RoughnessTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				{
				//	perDrawData.bToggleRoughnessTexture = false;
					perDrawData.RoughnessConstant = materials[j]->RoughnessConstant;
				}

				{
					perDrawData.bToggleNormalTexture = true;
					normalTextureHandle = materials[j]->NormalTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				{
				//	perDrawData.bToggleNormalTexture = false;
				}
				

				perDrawData.bToggleIrradianceTexture = true;

				m_PerDrawConstantBuffers[i * 5 + j]->SetData((void*)&perDrawData, 0, sizeof(perDrawData));
				device->CopyDescriptorsSimple(1, destHeapHandle, m_PerDrawConstantBuffers[i * 5 + j]->GetConstantBufferView(),
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



