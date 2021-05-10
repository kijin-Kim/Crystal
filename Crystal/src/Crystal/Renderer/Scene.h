#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/Components/TransformComponent.h"

namespace Crystal {

	struct Scene
	{
		std::unordered_map<EShadingModel, std::unordered_map<
			                   std::string, std::vector<std::weak_ptr<PrimitiveComponent>>>> PrimitiveComponents;


		std::vector<std::weak_ptr<LightComponent>> LightComponents;
		std::vector<std::weak_ptr<CameraComponent>> CameraComponents;

		const float MaxStaledTime = 5.0f;
		Timer StaleComponentsTimer;


		void AddPrimitive(const std::shared_ptr<PrimitiveComponent>& primitive);
		void AddLight(const std::shared_ptr<LightComponent>& light);
		void AddCamera(const std::shared_ptr<CameraComponent>& camera);

		const std::vector<std::weak_ptr<LightComponent>>& GetLights();
		const std::vector<std::weak_ptr<CameraComponent>>& GetCameras();
		const std::vector<std::weak_ptr<PrimitiveComponent>>& GetPrimitives(
			EShadingModel shadingModel, std::string type);
		const std::unordered_map<std::string, std::vector<std::weak_ptr<PrimitiveComponent>>>& GetPrimitives(
			EShadingModel shadingModel);

		void RemoveStaledComponents();

		void Update();
	};

}
