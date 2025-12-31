#pragma once
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"


namespace Crystal {
	class TextureComponent;
}

class PlayerShield : public Crystal::Actor
{

public:
	PlayerShield() = default;
	~PlayerShield() override = default;

	void Initialize() override;

	void Update(float deltaTime) override;

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	void SetShowHealthBar(bool bShow);

	void UpdateHealth() const;


	STATIC_TYPE_IMPLE(PlayerShield)

private:

	const float m_MaxHealth = 10.0f;
	float m_CurrentHealth = m_MaxHealth;

	Crystal::Shared<Crystal::TextureComponent> m_HealthBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealthBarBgComponent = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;


	Crystal::Timer m_HealthBarShowTimer = {};
	const float m_HealthBarShowTime = 3.0f;
	bool m_bShouldShowHealthBar = false;

	
};


