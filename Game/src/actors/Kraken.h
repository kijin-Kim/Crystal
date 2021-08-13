#pragma once
#include "PolluteSphere.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Objects/Actors/PostProcessVolumeActor.h"

namespace Crystal {
	class TextureComponent;
	class BoundingOrientedBoxComponent;
}

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

	uint32_t GetPhase() const { return m_CurrentPhase; }
	void SetPhase(uint32_t phase);

	void SetPolluteGauge(float polluteGauge);

	float GetCurrentPolluteGauge() const { return m_CurrentPolluteGauge; }

	void SetShowHealthBar(bool bShow);
	

	STATIC_TYPE_IMPLE(Kraken)

private:
	Crystal::Shared<Crystal::SkeletalMeshComponent> m_SkeletalMeshComponent = nullptr;
	Crystal::Shared<Crystal::BoundingOrientedBoxComponent> m_ImpulseBoxComponent = nullptr;
	Crystal::Timer m_SpaceWhaleSpawnTimer = {};
	float m_SpaceWhaleSpawnInterval = 5.0f;
	float m_CurrentHealth;
	float m_MaxHealth = 600.0f;

	uint32_t m_CurrentPhase = 1;

	uint32_t m_CurrentKrakenSpawnCount = 0;
	uint32_t m_MaxKrakenSpawnCount = 1;


	float m_CurrentPolluteGauge = 0.0f;
	const float m_MaxPolluteGauge = 300.0f;

	Crystal::Timer m_PolluteDamageTimer = {};
	const float m_PolluteDamageInterval = 2.5f;


	Crystal::Shared<Crystal::TextureComponent> m_HealthBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealthBarBgComponent = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;


	Crystal::Timer m_HealthBarShowTimer = {};
	const float m_HealthBarShowTime = 3.0f;
	bool m_bShouldShowHealthBar = false;

public:
	Crystal::Shared<Crystal::PostProcessVolumeActor> GreenTintVolumeActor;
	Crystal::Shared<Crystal::PostProcessVolumeActor> VignetteVolumeActor;
};


