#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

class SpaceStation : public Crystal::Actor
{
public:
	SpaceStation() = default;
	~SpaceStation() override = default;

	void Initialize() override;

	STATIC_TYPE_IMPLE(SpaceStation)
};
