#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

class Kraken : public Crystal::Pawn
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Pawn>(*this);
	}

public:
	void Initialize() override;
	void Begin() override;
	
	void Update(float deltaTime) override;
	
	void UpdateHealth();
	
	void OnAttack();
	void OnIdle();

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;


	STATIC_TYPE_IMPLE(Kraken)

private:
	Crystal::Shared<Crystal::SkeletalMeshComponent> m_SkeletalMeshComponent = nullptr;
	Crystal::Timer m_SpaceWhaleSpawnTimer = {};
	float m_SpaceWhaleSpawnInterval = 5.0f;
	float m_Health;
	float m_MaxHealth = 300.0f;
};

