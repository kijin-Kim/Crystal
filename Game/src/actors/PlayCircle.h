#pragma once
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Objects/Actors/LightActor.h"

class PlayCircle : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	PlayCircle() = default;
	~PlayCircle() override = default;

	void Initialize() override
	{
		Crystal::Actor::Initialize();


		auto material = std::make_unique<Crystal::Material>();
		material->AlbedoColor = Crystal::Vector3::Red;


		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");

		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->SetScale(600.0f);
		

		m_MainComponent = staticMeshComponent;
	}

	STATIC_TYPE_IMPLE(PlayCircle)
};
