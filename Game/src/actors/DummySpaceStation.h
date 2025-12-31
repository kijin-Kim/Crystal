#pragma once
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

class DummySpaceStation : public Crystal::Pawn
{
public:
	DummySpaceStation() = default;
	~DummySpaceStation() override = default;

	void Initialize() override;

	void Begin() override;

	void UpdateHealth();
	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	float GetHealth() const { return m_CurrentHealth; }

	STATIC_TYPE_IMPLE(DummySpaceStation)

private:

	Crystal::Shared<Crystal::AIPerceptionSourceComponent> m_AIPerceptionSourceComponent = nullptr;

	float m_MaxHealth = 600.0f;
	float m_CurrentHealth = m_MaxHealth;
};



