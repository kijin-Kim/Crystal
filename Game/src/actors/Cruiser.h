#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

class Cruiser : public Crystal::Pawn
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	Cruiser() = default;
	~Cruiser() override = default;

	void Initialize() override;

	void Update(float deltaTime) override;

	STATIC_TYPE_IMPLE(Cruiser)

};

