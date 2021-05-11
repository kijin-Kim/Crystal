#include "cspch.h"
#include "Scene.h"

namespace Crystal {


	void Scene::AddPrimitive(const std::shared_ptr<PrimitiveComponent>& primitive)
	{
		auto shadingModel = primitive->GetMaterial(0)->ShadingModel;
		auto type = primitive->StaticType();


		PrimitiveComponents[shadingModel][type].push_back(primitive);
	}

	void Scene::AddLight(const std::shared_ptr<LightComponent>& light)
	{
		LightComponents.push_back(light);
	}

	void Scene::AddCamera(const std::shared_ptr<CameraComponent>& camera)
	{
		CameraComponents.push_back(camera);
	}

	void Scene::RemoveStaledComponents()
	{
		for (auto& model : PrimitiveComponents)
		{
			for (auto& type : model.second)
			{
				for (auto it = type.second.begin(); it != type.second.end();)
				{
					if (it->expired())
					{
						it = type.second.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
		}
	}

	void Scene::Update()
	{
		StaleComponentsTimer.Tick();

		if (StaleComponentsTimer.GetElapsedTime() >= MaxStaledTime)
		{
			StaleComponentsTimer.Reset();

			RemoveStaledComponents();
		}
	}
}
