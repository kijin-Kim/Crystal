#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"



class PlayerShield : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	PlayerShield() = default;
	~PlayerShield() override = default;

	void Initialize() override;


	STATIC_TYPE_IMPLE(PlayerShield)
};
