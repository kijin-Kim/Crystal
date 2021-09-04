#include "Inventory.h"

#include "MyHUD.h"
#include "MyPlayerPawn.h"
#include "Crystal/GamePlay/World/Level.h"

void Inventory::AddGold(uint32_t amount)
{
	m_Gold += amount;
	OnGoldUpdated();
}

void Inventory::SubtractGold(uint32_t amount)
{
	m_Gold -= amount;
	OnGoldUpdated();
}

void Inventory::OnGoldUpdated()
{
	auto playerPawn = Crystal::Cast<MyPlayerPawn>(GetOuter());
	if(!playerPawn)
	{
		return;
	}

	auto level = playerPawn->GetLevel().lock();
	if(!level)
	{
		return;
	}

	auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
	hud->OnGoldUpdated(m_Gold);
}

