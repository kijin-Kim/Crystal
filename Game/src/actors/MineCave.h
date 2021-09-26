#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

class MineCave : public Crystal::Actor
{
public:
	MineCave() = default;
	~MineCave() override = default;

	void Initialize() override;

};
