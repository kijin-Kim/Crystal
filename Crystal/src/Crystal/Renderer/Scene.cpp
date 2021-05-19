#include "cspch.h"
#include "Scene.h"

#include "Crystal/Core/Device.h"
#include "SceneConstantBufferData.h"

namespace Crystal {


	Scene::Scene()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = MaxSceneDescriptorCount;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&SceneDescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		/*
		 * Descriptor Heap에 앞 절반은 ConstantBuffer,
		 * 나머지 절반은 Texture을 위해서 사용.
		 */

		for (uint64_t i = 0; i < MaxSceneDescriptorCount; i++)
		{
			FreeTextureHandleOffsets.push(i);
		}
	}


	Scene::OffsetType Scene::GetNewTextureHandleOffset()
	{
		OffsetType returnValue = FreeTextureHandleOffsets.front();
		FreeTextureHandleOffsets.pop();

		return returnValue;
	}

	void Scene::AddStaticMesh(const Shared<StaticMeshComponent>& staticMesh)
	{
		auto device = Device::Instance().GetD3DDevice();

		StaticMeshes.push_back(staticMesh);


		AddConstantBuffer(staticMesh, sizeof(SceneStaticMeshConstants));


		// 메터리얼을 복사합니다.
		auto materials = staticMesh->GetMaterials();
		for (auto& material : materials)
		{
			if (ShouldAddTexture(material->AlbedoTexture))
			{
				AddTexture(material->AlbedoTexture);
			}

			if (ShouldAddTexture(material->MetallicTexture))
			{
				AddTexture(material->MetallicTexture);
			}

			if (ShouldAddTexture(material->RoughnessTexture))
			{
				AddTexture(material->RoughnessTexture);
			}

			if (ShouldAddTexture(material->NormalTexture))
			{
				AddTexture(material->NormalTexture);
			}

			if (ShouldAddTexture(material->EmissiveTexture))
			{
				AddTexture(material->EmissiveTexture);
			}
		}
	}

	void Scene::AddConstantBuffer(const Shared<TransformComponent>& component, uint64_t size)
	{
		ConstantBuffers[component] = CreateShared<Buffer>(nullptr, sizeof(size), 0, true, true);
		ConstantBuffers[component]->CreateConstantBuffer();
	}

	void Scene::AddTexture(const Shared<Texture>& texture)
	{
		auto device = Device::Instance().GetD3DDevice();

		TextureHandleOffsets[texture] = GetNewTextureHandleOffset();

		auto albedoTextureDestHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(SceneDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		albedoTextureDestHandle.Offset(TextureHandleOffsets[texture]);

		device->CopyDescriptorsSimple(1, albedoTextureDestHandle, texture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	bool Scene::ShouldAddTexture(const Shared<Texture>& texture)
	{
		return texture && TextureHandleOffsets.count(texture) == 0;
	}

	void Scene::SetMainCamera(const Shared<CameraComponent>& camera)
	{
		auto it = std::find_if(Cameras.begin(), Cameras.end(), [&camera](const Weak<CameraComponent>& other)-> bool
		{
			return other.lock() == camera;
		});

		if (it != Cameras.begin())
		{
			// 카메라가 이미 존재하고 메인 카메라(Camera[0])이므로 아무것도 할 필요가 없음.
			return;
		}


		if (it == Cameras.end())
		{
			Cameras.insert(Cameras.begin(), camera);
		}
		else
		{
			Cameras[0] = camera;
		}
	}

	void Scene::RemoveGarbage()
	{
		for (auto it = StaticMeshes.begin(); it != StaticMeshes.end();)
		{
			if (it->expired())
			{
				it = StaticMeshes.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (auto it = SkeletalMeshes.begin(); it != SkeletalMeshes.end();)
		{
			if (it->expired())
			{
				it = SkeletalMeshes.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (auto it = Particles.begin(); it != Particles.end();)
		{
			if (it->expired())
			{
				it = Particles.erase(it);
			}
			else
			{
				++it;
			}
		}


		for (auto it = Lights.begin(); it != Lights.end();)
		{
			if (it->expired())
			{
				it = Lights.erase(it);
			}
			else
			{
				++it;
			}
		}


		for (auto it = Cameras.begin(); it != Cameras.end();)
		{
			if (it->expired())
			{
				it = Cameras.erase(it);
			}
			else
			{
				++it;
			}
		}
	}


}
;
