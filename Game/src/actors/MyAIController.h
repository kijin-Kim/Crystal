#pragma once
#include "Crystal/GamePlay/Controllers/AIController.h"

class MyAIController : public Crystal::AIController
{
public:
	MyAIController() = default;
	~MyAIController() override = default;

	void Update(float deltaTime) override
	{
		Crystal::AIController::Update(deltaTime);


		
	}
	

	STATIC_TYPE_IMPLE(MyAIController)
};
