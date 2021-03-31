#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {
	class SkyboxActor : public Actor
	{
	public:
		SkyboxActor()
		{
			auto& resourceManager = ResourceManager::Instance();

			auto staticMeshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable("PlaneQuadMesh"));
		}

		~SkyboxActor() override = default;
	};
}
