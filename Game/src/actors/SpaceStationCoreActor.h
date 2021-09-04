#pragma once


#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

class SpaceStationCoreActor : public Crystal::Actor
{
public:
	SpaceStationCoreActor() = default;
	~SpaceStationCoreActor() override = default;

	void Initialize() override;
	STATIC_TYPE_IMPLE(SpaceStationCoreActor)
};
