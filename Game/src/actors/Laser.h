#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Objects/Actors/LightActor.h"

class Laser : public Crystal::PointLightActor
{
public:
	Laser() = default;
	~Laser() override = default;

	void Initialize() override;

	void Update(float deltaTime) override;


	STATIC_TYPE_IMPLE(Laser)

private:
	Crystal::Timer m_LaserLifeTimer = {};
	float m_LaserLifeTime = 2.0f;
};

