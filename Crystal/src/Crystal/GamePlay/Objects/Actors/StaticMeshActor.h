#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/TransformComponent.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {

	class StaticMeshActor : public Actor
	{

	public:
		StaticMeshActor() = default;
		~StaticMeshActor() override = default;

		void Initialize() override
		{
			auto meshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent");
			m_MainComponent = meshComponent;
		}

		STATIC_TYPE_IMPLE(StaticMeshActor)
	};
}
