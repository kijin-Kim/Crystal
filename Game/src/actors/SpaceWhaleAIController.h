#pragma once

#include "Crystal/GamePlay/Controllers/AIController.h"

class SpaceWhaleAIController : public Crystal::AIController
{
public:
	SpaceWhaleAIController() = default;
	~SpaceWhaleAIController() override = default;

	STATIC_TYPE_IMPLE(SpaceWhaleAIController)
};
