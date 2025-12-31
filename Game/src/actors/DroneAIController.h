#pragma once
#include "Crystal/GamePlay/Controllers/AIController.h"

class DroneAIController : public Crystal::AIController
{
public:
	DroneAIController() = default;
	~DroneAIController() override = default;

	void Initialize() override;
	void Begin() override;
	void Update(float deltaTime) override;

	STATIC_TYPE_IMPLE(DroneAIController)

private:
	float m_RoundPerMin = 100.0f;
	float m_RoundPerSec = m_RoundPerMin / 60.0f;
	float m_FireInterval = 1.0f / m_RoundPerSec;
};



