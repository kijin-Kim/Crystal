#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"



class Laser : public Crystal::Actor
{

public:
	Laser() = default;
	~Laser() override = default;

	void Initialize() override;


	STATIC_TYPE_IMPLE(Laser)
};

