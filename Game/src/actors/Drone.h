#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

namespace Crystal {
	class TextureComponent;
}

class Drone : public Crystal::Pawn
{
public:
	Drone() = default;
	~Drone() override = default;

	void Initialize() override;

	void Update(float deltaTime) override;

	void OnFire();

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	void SetShowHealthBar(bool bShow);

	void UpdateHealth() const;


	STATIC_TYPE_IMPLE(Drone)

private:
	Crystal::Shared<Crystal::TransformComponent> m_LeftFireSocketComponent = nullptr;
	Crystal::Shared<Crystal::TransformComponent> m_RightFireSocketComponent = nullptr;


	const float m_MaxHealth = 15.0f;
	float m_CurrentHealth = m_MaxHealth;

	Crystal::Shared<Crystal::TextureComponent> m_HealthBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealthBarBgComponent = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;


	Crystal::Timer m_HealthBarShowTimer = {};
	const float m_HealthBarShowTime = 3.0f;
	bool m_bShouldShowHealthBar = false;
};
