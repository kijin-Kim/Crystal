#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/Components/TransformComponent.h"
#include "Crystal/Resources/BasicShapeMeshes.h"

namespace Crystal {

	/*
	 * 렌더링에 사용되는 리소스들을 준비합니다.
	 */

	struct Scene
	{
		/*
		 * VertexBuffers
		 * IndexBuffer
		 * Transform
		 * Material
		 * ShadingModel
		 */

		Scene()
		{
		}


		Scene& operator=(const Scene&) = delete;
		Scene(const Scene&) = delete;


		Shared<Line> LineMesh = CreateShared<Line>();
		Shared<LineBox> LineBoxMesh = CreateShared<LineBox>();
		Shared<LineSphere> LineSphereMesh = CreateShared<LineSphere>();
		Shared<PlaneQuad2D> PlaneQuad2DMesh = CreateShared<PlaneQuad2D>();
		Shared<PlaneQuad3D> PlaneQuad3DMesh = CreateShared<PlaneQuad3D>();
		Shared<PlaneQuad3DTextured> PlaneQuad3DTexturedMesh = CreateShared<PlaneQuad3DTextured>();


		std::unordered_map<EShadingModel, std::unordered_map<
			                   std::string, std::vector<Weak<PrimitiveComponent>>>> PrimitiveComponents;

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


		std::vector<Weak<LightComponent>> LightComponents;
		std::vector<Weak<CameraComponent>> CameraComponents;

		void AddPrimitive(const Shared<PrimitiveComponent>& primitive);
		void AddLight(const Shared<LightComponent>& light);
		void AddCamera(const Shared<CameraComponent>& camera);


		void RemoveStaledComponents();

		void Update();


	private:
		const float MaxStaledTime = 5.0f;
		Timer StaleComponentsTimer;
	};

}
