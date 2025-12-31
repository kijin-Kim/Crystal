#include "Inventory.h"

#include "MyHUD.h"
#include "MyPlayerPawn.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Components/ButtonComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/Material.h"

void Inventory::Initialize()
{
	auto& resourceManager = Crystal::ResourceManager::Instance();

	

	m_ItemThumbNails[EItemType::IT_PowerPotion] = Crystal::CreateShared<Crystal::Material>();
	m_ItemThumbNails[EItemType::IT_PowerPotion]->AlbedoTexture = resourceManager.GetTexture("assets/textures/electric_potion.tga");
	m_ItemThumbNails[EItemType::IT_PowerPotion]->bUseAlbedoTextureAlpha = true;

	m_ItemThumbNails[EItemType::IT_HealPotion] = Crystal::CreateShared<Crystal::Material>();
	m_ItemThumbNails[EItemType::IT_HealPotion]->AlbedoTexture = resourceManager.GetTexture("assets/textures/potion_heal.tga");
	m_ItemThumbNails[EItemType::IT_HealPotion]->bUseAlbedoTextureAlpha = true;

	m_ItemThumbNails[EItemType::IT_ShieldPotion] = Crystal::CreateShared<Crystal::Material>();
	m_ItemThumbNails[EItemType::IT_ShieldPotion]->AlbedoTexture = resourceManager.GetTexture("assets/textures/shield_potion.tga");
	m_ItemThumbNails[EItemType::IT_ShieldPotion]->bUseAlbedoTextureAlpha = true;

	m_ItemThumbNails[EItemType::IT_Ore] = Crystal::CreateShared<Crystal::Material>();
	m_ItemThumbNails[EItemType::IT_Ore]->AlbedoTexture = resourceManager.GetTexture("assets/textures/diamond1.tga");
	m_ItemThumbNails[EItemType::IT_Ore]->bUseAlbedoTextureAlpha = true;



	m_ItemCost[EItemType::IT_PowerPotion] = 400.0f;
	m_ItemCost[EItemType::IT_HealPotion] = 100.0f;
	m_ItemCost[EItemType::IT_ShieldPotion] = 200.0f;
	m_ItemCost[EItemType::IT_Ore] = 20.0f;
}

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
	auto playerPawn = Crystal::Cast<MyPlayerPawn>(GetInstigator());
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



uint32_t Inventory::GetItemCount(EItemType item)
{
	return m_Items[item];
}

void Inventory::AddItem(EItemType item)
{
	m_Items[item]++;
}

void Inventory::RemoveItem(EItemType item)
{
	auto it = m_Items.find(item);
	if(it != m_Items.end() && it->second > 0)
	{
		m_Items[item]--;
	}
}

Crystal::Shared<Crystal::Material> Inventory::GetItemThumbNails(EItemType itemType)
{
	return m_ItemThumbNails[itemType];
}

uint32_t Inventory::GetItemCost(EItemType type)
{
	return m_ItemCost[type];
}



