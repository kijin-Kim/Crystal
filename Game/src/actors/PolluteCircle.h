#pragma once
#include "Kraken.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/ResourceManager.h"

class PolluteCircle : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	PolluteCircle() = default;
	~PolluteCircle() override = default;

	void Initialize() override;


	STATIC_TYPE_IMPLE(PolluteCircle)


private:
	float m_Health = 150.0f;
	Crystal::Shared<Crystal::StaticMeshComponent> m_StaticMeshComponent = nullptr;
};
