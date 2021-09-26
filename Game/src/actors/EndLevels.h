#pragma once
#include "EndLevelPawns.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"


class GameOverLevel : public Crystal::Level
{
public:
	GameOverLevel() = default;
	~GameOverLevel() override = default;

	void OnLevelOpened(Crystal::Shared<Level> lastLevel) override
	{
		Crystal::Level::OnLevelOpened(lastLevel);

		//m_HUD = SpawnActor<MyHUD>({ "" }).lock();
		m_Player = SpawnActor<GameOverLevelPawn>({ "GameOverLevelPawn" }).lock();
		m_Player->SetPosition({ 0.0f, 0.0f, -1.0f });
		auto playerController = SpawnActor<Crystal::PlayerController>({}).lock();
		playerController->Possess(m_Player);

	}

	STATIC_TYPE_IMPLE(GameOverLevel)
};

class GameClearLevel : public Crystal::Level
{
public:
	GameClearLevel() = default;
	~GameClearLevel() override = default;

	void OnLevelOpened(Crystal::Shared<Level> lastLevel) override
	{
		Crystal::Level::OnLevelOpened(lastLevel);

		//m_HUD = SpawnActor<MyHUD>({ "" }).lock();
		m_Player = SpawnActor<GameClearLevelPawn>({ "GameClearLevelPawn" }).lock();
		m_Player->SetPosition({ 0.0f, 0.0f, -1.0f });
		auto playerController = SpawnActor<Crystal::PlayerController>({}).lock();
		playerController->Possess(m_Player);

	}

	STATIC_TYPE_IMPLE(GameClearLevel)
};
