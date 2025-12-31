#pragma once
#include "Crystal/GamePlay/Controllers/AIController.h"


class AllyDroneAIController : public Crystal::AIController
{
public:
	AllyDroneAIController() = default;
	~AllyDroneAIController() override = default;

	void Initialize() override;
	void Begin() override;

	void OnPosses(Crystal::Shared<Crystal::Pawn> pawn) override;


	STATIC_TYPE_IMPLE(AllyDroneAIController)

private:
	float m_RoundPerMin = 500.0f;
	float m_RoundPerSec = m_RoundPerMin / 60.0f;
	float m_FireInterval = 1.0f / m_RoundPerSec;
};


