#pragma once
#include "Crystal/GamePlay/Controllers/AIController.h"

class KrakenAIController : public Crystal::AIController
{
public:
	KrakenAIController() = default;
	~KrakenAIController() override = default;

	void Update(float deltaTime) override
	{
		Crystal::AIController::Update(deltaTime);		
	}
	
	STATIC_TYPE_IMPLE(KrakenAIController)
};
