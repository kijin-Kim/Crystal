#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/Components/TransformComponent.h"
#include "Crystal/Resources/BasicShapeMeshes.h"

namespace Crystal {

	struct Scene
	{
		/*
		 * VertexBuffers
		 * IndexBuffer
		 * Transform
		 * Material
		 * ShadingModel
		 */


		Shared<Line> LineMesh = CreateShared<Line>();
		Shared<LineBox> LineBoxMesh = CreateShared<LineBox>();
		Shared<LineSphere> LineSphereMesh = CreateShared<LineSphere>();
		Shared<PlaneQuad2D> PlaneQuad2DMesh = CreateShared<PlaneQuad2D>();
		Shared<PlaneQuad3D> PlaneQuad3DMesh = CreateShared<PlaneQuad3D>();
		Shared<PlaneQuad3DTextured> PlaneQuad3DTexturedMesh = CreateShared<PlaneQuad3DTextured>();


		std::unordered_map<EShadingModel, std::unordered_map<
			                   std::string, std::vector<std::weak_ptr<PrimitiveComponent>>>> PrimitiveComponents;


		std::vector<std::weak_ptr<LightComponent>> LightComponents;
		std::vector<std::weak_ptr<CameraComponent>> CameraComponents;


		void AddPrimitive(const std::shared_ptr<PrimitiveComponent>& primitive);
		void AddLight(const std::shared_ptr<LightComponent>& light);
		void AddCamera(const std::shared_ptr<CameraComponent>& camera);


		void RemoveStaledComponents();

		void Update();


	private:
		const float MaxStaledTime = 5.0f;
		Timer StaleComponentsTimer;
	};

}
