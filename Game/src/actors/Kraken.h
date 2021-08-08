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

	uint32_t GetPhase() const { return m_CurrentPhase;  }
	void SetPhase(uint32_t phase);


	STATIC_TYPE_IMPLE(Kraken)

private:
	Crystal::Shared<Crystal::SkeletalMeshComponent> m_SkeletalMeshComponent = nullptr;
	Crystal::Timer m_SpaceWhaleSpawnTimer = {};
	float m_SpaceWhaleSpawnInterval = 5.0f;
	float m_Health;
	float m_MaxHealth = 300.0f;

	uint32_t m_CurrentPhase = 1;

	uint32_t m_CurrentKrakenSpawnCount = 0;
	uint32_t m_MaxKrakenSpawnCount = 0;
};

