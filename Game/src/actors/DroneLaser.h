#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

class DroneLaser : public Crystal::Actor
{
public:
	DroneLaser() = default;
	~DroneLaser() override = default;

	void Initialize() override;

	void Update(float deltaTime) override;


	STATIC_TYPE_IMPLE(DroneLaser)

private:
	Crystal::Timer m_LaserLifeTimer = {};
	float m_LaserLifeTime = 2.0f;
	float m_Damage = 1.0f;
};
