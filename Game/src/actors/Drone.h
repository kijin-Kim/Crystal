#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

class Drone : public Crystal::Pawn
{
public:
	Drone() = default;
	~Drone() override = default;

	void Initialize() override;

	void OnFire();


	STATIC_TYPE_IMPLE(Drone)

private:
	Crystal::Shared<Crystal::TransformComponent> m_FireSocketComponent = nullptr;
};
