#pragma once

#include "Crystal/GamePlay/Controllers/AIController.h"

class SpaceWhaleAIController : public Crystal::AIController
{
public:
	SpaceWhaleAIController() = default;
	~SpaceWhaleAIController() override = default;

	void Initialize() override;

	void Begin() override;


	STATIC_TYPE_IMPLE(SpaceWhaleAIController)
};


