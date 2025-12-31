#pragma once
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

class Cruiser : public Crystal::Pawn
{

public:
	Cruiser() = default;
	~Cruiser() override = default;

	void Begin() override;
	void Initialize() override;

	void Update(float deltaTime) override;


	void UpdateHealth();
	void OnCruiserMoving();
	void OnCruiserStopped();
	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	float GetHealth() const { return m_CurrentHealth; }


	STATIC_TYPE_IMPLE(Cruiser)

private:
	Crystal::Shared<Crystal::AIPerceptionSourceComponent> m_AIPerceptionSourceComponent = nullptr;
	Crystal::Shared<Crystal::BoundingSphereComponent> m_OuterSphereComponent = nullptr;
	float m_MaxHealth = 600.0f;
	float m_CurrentHealth = m_MaxHealth;


};



