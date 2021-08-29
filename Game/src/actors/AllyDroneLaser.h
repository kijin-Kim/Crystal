#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

class AllyDroneLaser : public Crystal::Actor
{
public:
	AllyDroneLaser() = default;
	~AllyDroneLaser() override = default;



	void Initialize() override;
	void Update(float deltaTime) override;

	STATIC_TYPE_IMPLE(AllyDroneLaser)

private:

	Crystal::Timer m_LaserLifeTimer = {};
	float m_LaserLifeTime = 2.0f;
	float m_Damage = 10.0f;
};
