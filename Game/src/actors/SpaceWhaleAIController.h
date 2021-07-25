#pragma once

#include "Crystal/GamePlay/Controllers/AIController.h"

class SpaceWhaleAIController : public Crystal::AIController
{
public:
	SpaceWhaleAIController() = default;
	~SpaceWhaleAIController() override = default;

	void Update(float deltaTime) override
	{
		Crystal::AIController::Update(deltaTime);



	}

	STATIC_TYPE_IMPLE(SpaceWhaleAIController)
};
