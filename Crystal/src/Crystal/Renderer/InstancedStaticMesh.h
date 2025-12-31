#pragma once
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/Meshes.h"

namespace Crystal {


	struct StaticMeshPerInstanceData
	{
		DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();
		DirectX::XMFLOAT3 AlbedoColor = Vector3::Zero;
		DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;
		float RoughnessConstant = 0.0f;
		float MetallicConstant = 0.0f;

		uint32_t bToggleAlbedoTexture = false;
		uint32_t bToggleMetallicTexture = false;
		uint32_t bToggleRoughnessTexture = false;
		uint32_t bToggleNormalTexture = false;
		uint32_t bToggleIrradianceTexture = false;
		uint32_t bToggleEmissivetexture = false;
	};
	
	struct InstancedStaticMeshData
	{
		std::vector<StaticMeshPerInstanceData> PerInstanceDatas;
		Shared<Buffer> PerInstanceVertexBuffer = nullptr;
		Material* Material = nullptr;
		UINT64 DescriptorOffset = -1;

		void AddPerInstanceData(const StaticMeshPerInstanceData& perInstanceData)
		{
			PerInstanceDatas.push_back(perInstanceData);
		}

		bool HasMaterial() const
		{
			return Material != nullptr;
		}
	};

	struct InstancedStaticMesh
	{
		std::map<Renderable*, InstancedStaticMeshData> InstancedStaticMeshes;
		uint32_t MaterialCount = 0;
		

		void AddInstance(const Shared<StaticMeshComponent>& meshComponent, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
			uint32_t descriptorTableOffset);

		void PrepareDynamicVertexBuffer();
		void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t rootParameterIndex);

		void Clear();
	};

	
}


