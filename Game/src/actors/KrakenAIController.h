#pragma once
#include "Crystal/GamePlay/Controllers/AIController.h"

class KrakenAIController : public Crystal::AIController
{
public:
	KrakenAIController() = default;
	~KrakenAIController() override = default;

	void Initialize() override;
	void Begin() override;
	void Update(float deltaTime) override;

	STATIC_TYPE_IMPLE(KrakenAIController)
};


