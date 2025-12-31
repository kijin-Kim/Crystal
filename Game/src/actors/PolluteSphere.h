#pragma once
#include "Kraken.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/ResourceManager.h"

class PolluteSphere : public Crystal::Actor
{

public:
	PolluteSphere() = default;
	~PolluteSphere() override = default;

	void Initialize() override;


	STATIC_TYPE_IMPLE(PolluteSphere)


private:
	Crystal::Shared<Crystal::StaticMeshComponent> m_StaticMeshComponent = nullptr;
	
};


