#pragma once
#include "TitleInterceptorActor.h"
#include "TitleLevelPawn.h"
#include "Crystal/GamePlay/Objects/Actors/StaticMeshActor.h"
#include "Crystal/GamePlay/World/Level.h"

class TitleLevel : public Crystal::Level
{
public:
	TitleLevel() = default;
	~TitleLevel() override = default;

	void Initialize() override
	{
		Level::Initialize();
		SpawnActor<Crystal::PlayerStartActor>({});
	}

	void OnLevelOpened(Crystal::Shared<Level> lastLevel) override
	{
		Crystal::Level::OnLevelOpened(lastLevel);

		//m_HUD = SpawnActor<MyHUD>({ "" }).lock();
		m_Player = SpawnActor<TitleLevelPawn>({ "TitleLevelPawn" }).lock();
		m_Player->SetPosition({0.0f, 0.0f, -1.0f});
		auto playerController = SpawnActor<Crystal::PlayerController>({}).lock();
		playerController->Possess(m_Player);

		
	}

	STATIC_TYPE_IMPLE(TitleLevel)
};


