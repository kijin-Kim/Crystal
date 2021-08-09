#pragma once
#include "Crystal/GamePlay/Controllers/AIController.h"

class DroneAIController : public Crystal::AIController
{
public:
	DroneAIController() = default;
	~DroneAIController() override = default;

	void Initialize() override;
	void Begin() override;


	STATIC_TYPE_IMPLE(DroneAIController)
};
