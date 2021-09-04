#pragma once
#include "Crystal/Resources/BasicShapeMeshes.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/ParticleComponent.h"
#include "Crystal/GamePlay/Components/TextureComponent.h""
#include "Crystal/GamePlay/Components/TextRenderingComponent.h"

namespace Crystal {
	class ButtonComponent;
	class PostProcessComponent;

	/*
	 * 렌더링에 사용되는 리소스들을 준비합니다.
	 */

	struct Scene
	{
		Scene() = default;
		~Scene() = default;

		Scene& operator=(const Scene&) = delete;
		Scene(const Scene&) = delete;


		//======================== Scene Owned Objects ======================================
		Shared<Line> LineMesh = CreateShared<Line>();
		Shared<LineBox> LineBoxMesh = CreateShared<LineBox>();
		Shared<LineQuad> LineQuadMesh = CreateShared<LineQuad>();
		Shared<LineSphere> LineSphereMesh = CreateShared<LineSphere>();
		Shared<PlaneQuad2D> PlaneQuad2DMesh = CreateShared<PlaneQuad2D>();
		Shared<PlaneQuad3D> PlaneQuad3DMesh = CreateShared<PlaneQuad3D>();
		Shared<PlaneQuad3DTextured> PlaneQuad3DTexturedMesh = CreateShared<PlaneQuad3DTextured>();

		Shared<Texture> ColorBufferTextures[2];
		Shared<Texture> DepthStencilBufferTexture = nullptr;
		Shared<Texture> BrightColorBuffer = nullptr;
		Shared<Texture> FloatingPointBuffer = nullptr;

		Shared<Texture> AlbedoBuffer = nullptr;
		Shared<Texture> RoughnessMetallicAoBuffer = nullptr;
		Shared<Texture> EmissiveBuffer = nullptr;
		Shared<Texture> WorldNormalBuffer = nullptr;
		Shared<Texture> IrradianceBuffer = nullptr;
		Shared<Texture> WorldPositionBuffer = nullptr;

		Shared<Texture> PanoramaCubeColorTexture = nullptr;
		Shared<Texture> CubemapColorTexture = nullptr;
		Shared<Texture> IrradianceTexture = nullptr;

		Shared<Texture> PanoramaCubeAlphaTexture = nullptr;
		Shared<Texture> CubemapAlphaTexture = nullptr;

		Shared<Texture> PanoramaStarFarTexture = nullptr;
		Shared<Texture> CubemapStarFarTexture = nullptr;

		Shared<Texture> PanoramaStarNearTexture = nullptr;
		Shared<Texture> CubemapStarNearTexture = nullptr;

		struct SkyboxConfig
		{
			DirectX::XMFLOAT4 AdditiveColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			float RedLevel = 1.0f;
			float GreenLevel = 1.0f;
			float BlueLevel = 1.0f;
			float Power = 0.8f;
			float Saturation = 1.1f;
			float NearStarDesaturation = 0.0f;
			float FarStarDesaturation = 0.0f;
		} SkyboxConfiguration;

		

	
		Shared<Texture> ShadowMapTexture = nullptr;
		Shared<Texture> ShadowMapDummyColorBuffer = nullptr;
		//===================================================================================

		//=====================Not Scene Owned Objects ======================================
		std::vector<Weak<StaticMeshComponent>> StaticMeshes;
		std::vector<Weak<SkeletalMeshComponent>> SkeletalMeshes;
		std::vector<Weak<ParticleComponent>> Particles;

		std::vector<Weak<BoundingOrientedBoxComponent>> BoundingOrientedBoxComponents;
		std::vector<Weak<BoundingSphereComponent>> BoundingSphereComponents;
		std::vector<Weak<RayComponent>> RayComponents;
		std::vector<Weak<BoundingFrustumComponent>> BoundingFrustumComponents;
		
		
		std::vector<Weak<TextureComponent>> Textures;
		std::vector<Weak<TextRenderingComponent>> Texts;
		std::vector<Weak<ButtonComponent>> Buttons;

		std::vector<Weak<PostProcessComponent>> PostProcesses;

		std::vector<Weak<LightComponent>> Lights;
		std::vector<Weak<CameraComponent>> Cameras;

		std::vector<Weak<AIPerceptionSourceComponent>> AIPerceptionSources;
		std::vector<Weak<AIPerceptionComponent>> AIPerceptions;

		
		
		//===================================================================================

		void SetMainCamera(const Shared<CameraComponent>& camera);


		void RemoveGarbage();

		void Reset();
	};

}
