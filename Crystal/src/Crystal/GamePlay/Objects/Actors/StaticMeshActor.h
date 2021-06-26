#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/TransformComponent.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {

	class StaticMeshActor : public Actor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Crystal::Actor>(*this);
		}

	public:
		StaticMeshActor() = default;
		~StaticMeshActor() override = default;

		void Initialize() override
		{
			auto meshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent");

			auto material = std::make_unique<Material>();
			meshComponent->AddMaterial(std::move(material));

			m_MainComponent = meshComponent;
		}

		STATIC_TYPE_IMPLE(StaticMeshActor)
	};
}
