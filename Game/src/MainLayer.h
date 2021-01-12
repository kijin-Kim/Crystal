#pragma once
#include <Crystal.h>

class MainLayer : public Crystal::Layer
{
public:
	MainLayer() = default;
	virtual ~MainLayer() = default;

	virtual void Enter() override
	{
		CS_LOG("MainLayer Enter");
	}
	virtual void Exit() override
	{
		CS_LOG("MainLayer Exit");
	}
	virtual void Update(float deltatTime) override
	{
	}
};
