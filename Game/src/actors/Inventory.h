#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include <map>

namespace Crystal {
	struct Material;
}

enum class EItemType
{
	IT_PowerPotion,
	IT_HealPotion,
	IT_ShieldPotion,
	IT_Ore,
};

class Inventory : public Crystal::Actor
{
public:
	Inventory() = default;
	~Inventory() override = default;

	void Initialize() override;


	void AddGold(uint32_t amount);
	void SubtractGold(uint32_t amount);
	void OnGoldUpdated();

	uint32_t GetItemCount(EItemType item);
	void AddItem(EItemType item);
	void RemoveItem(EItemType item);

	uint32_t GetGold() const { return m_Gold; }

	Crystal::Shared<Crystal::Material> GetItemThumbNails(EItemType itemType);

	const std::map<EItemType, uint32_t>& GetItems() const { return m_Items; }
	uint32_t GetItemCost(EItemType type);

	STATIC_TYPE_IMPLE(Inventory)
private:
	uint32_t m_Gold = 0;

	std::map<EItemType, uint32_t> m_Items;
	std::map<EItemType, uint32_t> m_ItemCost;
	std::map<EItemType, Crystal::Shared<Crystal::Material>> m_ItemThumbNails;
};


