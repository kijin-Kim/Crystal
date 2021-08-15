#pragma once
#include "TitleLevelPawn.h"
#include "Crystal/GamePlay/World/Level.h"

class TitleLevel : public Crystal::Level
{
public:
	TitleLevel() = default;
	~TitleLevel() override = default;

	void Initialize() override
	{
		Crystal::Level::Initialize();

		//m_HUD = SpawnActor<MyHUD>({ "" }).lock();
		m_Player = SpawnActor<TitleLevelPawn>({ "TitleLevelPawn" }).lock();
		m_Player->SetPosition({ 0.0f, 0.0f, -1.0f });
		auto playerController = SpawnActor<Crystal::PlayerController>({}).lock();
		playerController->Possess(m_Player);
	}
};
