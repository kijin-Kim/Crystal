#pragma once
#include "SpaceWhaleAIController.h"
#include "Crystal/GamePlay/AI/Blackboard.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {
	class TextureComponent;
}

class SpaceWhale : public Crystal::Pawn
{

public:
	void Initialize() override;


	void End() override;
	void Update(float deltaTime) override;

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;


	void SetShowHealthBar(bool bShow);

	void UpdateHealth() const;


	STATIC_TYPE_IMPLE(SpaceWhale)

private:
	float m_Power = 10.0f;
	Crystal::Timer m_PolluteGaugeIncrementTimer = {};
	float m_PolluteGaugeIncrementInterval = 0.5f;


	const float m_MaxHealth = 150.0f;
	float m_CurrentHealth = m_MaxHealth;

	Crystal::Shared<Crystal::TextureComponent> m_HealthBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealthBarBgComponent = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;


	Crystal::Timer m_HealthBarShowTimer = {};
	const float m_HealthBarShowTime = 3.0f;
	bool m_bShouldShowHealthBar = false;

	Crystal::Timer m_DamagePerceptionTimer = {};
	const float m_DamagePerceptionInterval = 2.0f;

};
