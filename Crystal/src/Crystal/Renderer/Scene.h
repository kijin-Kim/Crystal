#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/Components/TransformComponent.h"
#include "Crystal/Resources/BasicShapeMeshes.h"

namespace Crystal {

	/*
	 * 렌더링에 사용되는 리소스들을 준비합니다.
	 */

	struct Scene
	{
		Scene();
		~Scene() = default;

		Scene& operator=(const Scene&) = delete;
		Scene(const Scene&) = delete;


		//======================== Scene Owned Objects ======================================
		Shared<Line> LineMesh = CreateShared<Line>();
		Shared<LineBox> LineBoxMesh = CreateShared<LineBox>();
		Shared<LineSphere> LineSphereMesh = CreateShared<LineSphere>();
		Shared<PlaneQuad2D> PlaneQuad2DMesh = CreateShared<PlaneQuad2D>();
		Shared<PlaneQuad3D> PlaneQuad3DMesh = CreateShared<PlaneQuad3D>();
		Shared<PlaneQuad3DTextured> PlaneQuad3DTexturedMesh = CreateShared<PlaneQuad3DTextured>();

		Shared<Texture> ColorBufferTextures[2] = {};
		Shared<Texture> DepthStencilBufferTexture = nullptr;
		Shared<Texture> BrightColorBuffer = nullptr;
		Shared<Texture> FloatingPointBuffer = nullptr;

		Shared<Texture> AlbedoBuffer = nullptr;
		Shared<Texture> RoughnessMetallicAoBuffer = nullptr;
		Shared<Texture> EmissiveBuffer = nullptr;
		Shared<Texture> WorldNormalBuffer = nullptr;
		Shared<Texture> IrradianceBuffer = nullptr;
		Shared<Texture> WorldPositionBuffer = nullptr;

		Shared<Texture> PanoramaTexture = nullptr;
		Shared<Texture> CubemapTexture = nullptr;
		Shared<Texture> IrradianceTexture = nullptr;

		using OffsetType = uint64_t;

		std::map<Weak<TransformComponent>, Shared<Buffer>, std::owner_less<Weak<TransformComponent>>> ConstantBuffers;
		
		std::map<Weak<Texture>, OffsetType, std::owner_less<Weak<Texture>>> TextureHandleOffsets; // 이미 복사된 Textures
		std::queue<OffsetType> FreeTextureHandleOffsets;


		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SceneDescriptorHeap = nullptr;
		const uint64_t MaxSceneDescriptorCount = 1024;

		//===================================================================================

		void SetIrradianceTexture(const Shared<Texture>& texture);
		

		void AddStaticMesh(const Shared<StaticMeshComponent>& staticMesh);
		void AddConstantBuffer(const Shared<TransformComponent>& component, uint64_t size);
		void AddTexture(const Shared<Texture>& texture);

		OffsetType GetNewTextureHandleOffset();
		bool ShouldAddTexture(const Shared<Texture>& texture);
		
		

		//=====================Not Scene Owned Objects ======================================
		std::vector<Weak<StaticMeshComponent>> StaticMeshes;
		std::vector<Weak<SkeletalMeshComponent>> SkeletalMeshes;
		std::vector<Weak<ParticleComponent>> Particles;
		std::vector<Weak<CollisionComponent>> CollisionComponents;


		std::vector<Weak<LightComponent>> Lights;
		std::vector<Weak<CameraComponent>> Cameras;
		//===================================================================================

		void SetMainCamera(const Shared<CameraComponent>& camera);


		void RemoveGarbage();
	};

}
