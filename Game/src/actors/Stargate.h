#pragma once
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

class Stargate : public Crystal::Actor
{
public:
	Stargate() = default;
	~Stargate() override = default;

	void Initialize() override;

	STATIC_TYPE_IMPLE(Stargate)
private:
	Crystal::Shared<Crystal::StaticMeshComponent> m_CircleMeshComponent;
};
