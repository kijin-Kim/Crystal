#pragma once
#include "Crystal/GamePlay/Controllers/AIController.h"

class KrakenAIController : public Crystal::AIController
{
public:
	KrakenAIController() = default;
	~KrakenAIController() override = default;

	STATIC_TYPE_IMPLE(KrakenAIController)
};
