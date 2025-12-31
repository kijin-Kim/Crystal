#pragma once
#include "Kraken.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Objects/Actors/LightActor.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

class ShieldSphere : public Crystal::Actor
{

public:
	ShieldSphere() = default;
	~ShieldSphere() override = default;

	void Initialize() override;

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;


	void Update(float deltaTime) override;

	void SetShowHealthBar(bool bShow);

	void UpdateHealth() const;

	STATIC_TYPE_IMPLE(ShieldSphere)


private:
	const float m_MaxHealth = 300.0f;
	float m_CurrentHealth = m_MaxHealth;

	Crystal::Shared<Crystal::TextureComponent> m_HealthBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealthBarBgComponent = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;


	Crystal::Timer m_HealthBarShowTimer = {};
	const float m_HealthBarShowTime = 3.0f;
	bool m_bShouldShowHealthBar = false;

};
