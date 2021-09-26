#pragma once

#pragma once
#include "ShopLevelPawn.h"
#include "ShopWidget.h"
#include "StargateWidget.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"

class StargateLevel : public Crystal::Level
{
public:
	StargateLevel() = default;
	~StargateLevel() override = default;

	void Initialize() override
	{
		Level::Initialize();

		m_Player = SpawnActor<ShopLevelPawn>({}).lock();
		m_Player->SetPosition({0.0f, 0.0f, -10.0f});
		auto playerController = SpawnActor<Crystal::PlayerController>({}).lock();
		playerController->Possess(m_Player);


		auto widget = SpawnActor<StargateWidget>({});
	}

	void OnLevelOpened(Crystal::Shared<Level> lastLevel) override
	{
		Level::OnLevelOpened(lastLevel);
		m_PlayerControllers[0]->SetInputMode(Crystal::EInputMode::IM_UI);
	}

	void OnLevelClosed(Crystal::Shared<Level> nextLevel) override
	{
		Level::OnLevelClosed(nextLevel);
		m_PlayerControllers[0]->SetInputMode(Crystal::EInputMode::IM_Game);
	}


	STATIC_TYPE_IMPLE(StargateLevel)
};
