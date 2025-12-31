#pragma once
#include "ClearLevelWidget.h"
#include "ShopLevelPawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"

class LevelClearedLevel : public Crystal::Level
{
public:
	LevelClearedLevel() = default;
	~LevelClearedLevel() override = default;

	void Initialize() override
	{
		Level::Initialize();

		m_Player = SpawnActor<ShopLevelPawn>({}).lock();
		m_Player->SetPosition({0.0f, 0.0f, -10.0f});
		auto playerController = SpawnActor<Crystal::PlayerController>({}).lock();
		playerController->Possess(m_Player);
	}

	void OnLevelOpened(Crystal::Shared<Level> lastLevel) override
	{
		Level::OnLevelOpened(lastLevel);
		m_PlayerControllers[0]->SetInputMode(Crystal::EInputMode::IM_UI);

		m_ClearLevelWidget = SpawnActor<ClearLevelWidget>({}).lock();
		auto reward = m_ClearLevelWidget->GetReward();


		auto inventory = Crystal::Cast<Inventory>(GetActorByClass("Inventory"));
		inventory->AddGold(reward.Gold);

		for (int i = 0; i < 4; i++)
		{
			for(int j = 0; j < reward.ItemCount[i]; j++)
			{
				inventory->AddItem(reward.ItemType[i]);
			}
		}
	}

	void OnLevelClosed(Crystal::Shared<Level> nextLevel) override
	{
		Level::OnLevelClosed(nextLevel);
		m_PlayerControllers[0]->SetInputMode(Crystal::EInputMode::IM_Game);
		auto inventory = GetActorByClass("Inventory");
		MoveActorToLevel(inventory.lock(), nextLevel);

		m_ClearLevelWidget->Destroy();

	}
private:
	Crystal::Shared<ClearLevelWidget> m_ClearLevelWidget;
};


