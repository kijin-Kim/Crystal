#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"


class LevelLandscapeActor : public Crystal::Actor
{
public:
	LevelLandscapeActor() = default;
	~LevelLandscapeActor() override = default;

	void Initialize() override;

};
