#include "cspch.h"
#include "LightingSkeletalPipeline.h"

#include "GeometryStaticPipeline.h"
#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/DescriptorAllocator.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {

	void LightingSkeletalPipeline::Begin(const Shared<Scene>& scene)
	{
		RenderPipeline::Begin(scene);


		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData), sizeof(PerDrawData), 5);


		auto camera = scene->Cameras[0].lock();


		PerFrameData perFrameData = {};

		perFrameData.ViewProjection = Matrix4x4::Transpose(camera->GetViewProjection());
		auto camPos = camera->GetWorldPosition();
		perFrameData.CameraPositionInWorld = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f);
		perFrameData.LightPositionInWorld[0] = DirectX::XMFLOAT4(20000.0f, 20000.0f, 0.0f, 0.0f);
		perFrameData.LightPositionInWorld[1] = DirectX::XMFLOAT4(-20000.0f, 20000.0f, 0.0f, 0.0f);
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto device = Device::Instance().GetD3DDevice();

		auto destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->AsConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		auto irradianceTextureHandle = scene->IrradianceTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/
		for (int i = 0; i < scene->SkeletalMeshes.size(); i++)
		{
			auto component = scene->SkeletalMeshes[i].lock();
			if (!component)
				continue;

			if(component->GetHiddenInGame())
			{
				continue;
			}

			PerObjectData perObjectData = {};

			auto skeletalMesh = Cast<SkeletalMesh>(component->GetRenderable());

			

			perObjectData.World = Matrix4x4::Transpose(component->GetWorldTransform());
			auto boneMatrices = component->GetBoneTransforms();
			std::copy(boneMatrices.begin(), boneMatrices.end(), perObjectData.Bones); // TODO : 최적화 매우매우매우매우 비효율적
			m_PerObjectConstantBuffers[i]->SetData((void*)&perObjectData, 0, sizeof(perObjectData));

			D3D12_CPU_DESCRIPTOR_HANDLE perObjectConstantBufferHandle = m_PerObjectConstantBuffers[i]->AsConstantBufferView();
			device->CopyDescriptorsSimple(1, destHeapHandle, perObjectConstantBufferHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto& materials = component->GetMaterials();

			if (materials.empty())
				continue;

			if (materials[0]->ShadingModel != EShadingModel::SM_Lit || materials[0]->BlendMode != EBlendMode::BM_Opaque)
				continue;
			
			for (int j = 0; j < materials.size(); j++)
			{
				PerDrawData perDrawData = {};

				D3D12_CPU_DESCRIPTOR_HANDLE albedoTextureHandle = {}; // PerMaterial
				D3D12_CPU_DESCRIPTOR_HANDLE metallicTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE roughnessTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE normalTextureHandle = {};

				auto albedoTexture = materials[j]->AlbedoTexture.lock();
				auto metallicTexture = materials[j]->MetallicTexture.lock();
				auto roughnessTexture = materials[j]->RoughnessTexture.lock();
				auto normalTexture = materials[j]->NormalTexture.lock();

				if (albedoTexture)
				{
					perDrawData.bToggleAlbedoTexture = true;
					albedoTextureHandle = albedoTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.AlbedoColor = materials[j]->AlbedoColor;
				}

				if(metallicTexture)
				{
					perDrawData.bToggleMetallicTexture = true;
					metallicTextureHandle = materials[j]->MetallicTexture.lock()->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.bToggleMetallicTexture = false;
					perDrawData.MetallicConstant = materials[j]->MetallicConstant;
				}

				if(roughnessTexture)
				{
					perDrawData.bToggleRoughnessTexture = true;
					roughnessTextureHandle = roughnessTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.bToggleRoughnessTexture = false;
					perDrawData.RoughnessConstant = materials[j]->RoughnessConstant;
				}

				if(normalTexture)
				{
					perDrawData.bToggleNormalTexture = true;
					normalTextureHandle = normalTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D);
				}
				else
				{
					perDrawData.bToggleNormalTexture = false;
				}


				perDrawData.bToggleIrradianceTexture = true;

				m_PerDrawConstantBuffers[i * j]->SetData(&perDrawData, 0, sizeof(perDrawData));

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
