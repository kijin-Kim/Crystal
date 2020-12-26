#pragma once
#include <Crystal.h>

class MainState : public Crystal::State
{
public:
	MainState() = default;
	virtual ~MainState() = default;

	virtual void Enter() override
	{
		CS_LOG("MainState Enter");
	}
	virtual void Exit() override
	{
		CS_LOG("MainState Exit");
	}
	virtual void Update(float deltatTime) override
	{
		CS_LOG("MainState Update");
	}
};
