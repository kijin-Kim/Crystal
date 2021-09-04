#pragma once
#include "Crystal/GamePlay/Objects/Object.h"


class Inventory : public Crystal::Object
{
public:
	Inventory() = default;
	~Inventory() override = default;


	void AddGold(uint32_t amount);
	void SubtractGold(uint32_t amount);
	void OnGoldUpdated();

	uint32_t GetGold() const { return m_Gold; }

	STATIC_TYPE_IMPLE(Inventory)

private:
	uint32_t m_Gold = 0;
};
