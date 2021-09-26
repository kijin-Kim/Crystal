#pragma once
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

class DummyStargate : public Crystal::Actor
{
public:
	DummyStargate() = default;
	~DummyStargate() override = default;

	void Initialize() override;

	STATIC_TYPE_IMPLE(DummyStargate)
private:
	Crystal::Shared<Crystal::StaticMeshComponent> m_CircleMeshComponent;
};

