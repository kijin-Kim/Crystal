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

	void OnSmashAttack();
	void OnIdle();

	void AddImpulseOnSmash();

	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	uint32_t GetPhase() const { return m_CurrentPhase; }
	void SetPhase(uint32_t phase);

	void SetPolluteGauge(float polluteGauge);

	float GetCurrentPolluteGauge() const { return m_CurrentPolluteGauge; }

	void SetShowHealthBar(bool bShow);


	STATIC_TYPE_IMPLE(Kraken)

private:
	Crystal::Shared<Crystal::SkeletalMeshComponent> m_SkeletalMeshComponent = nullptr;
	Crystal::Shared<Crystal::BoundingOrientedBoxComponent> m_SmashAttackBoxComponent = nullptr;
	Crystal::Timer m_SpaceWhaleSpawnTimer = {};
	float m_SpaceWhaleSpawnInterval = 10.0f;
	float m_CurrentHealth;
	float m_MaxHealth = 600.0f;

	uint32_t m_CurrentPhase = 1;

	uint32_t m_CurrentWhaleSpawnCount = 0;
	uint32_t m_MaxWhaleSpawnCount = 10;


	float m_CurrentPolluteGauge = 0.0f;
	const float m_MaxPolluteGauge = 300.0f;

	Crystal::Timer m_PolluteDamageTimer = {};
	const float m_PolluteDamageInterval = 2.5f;


	Crystal::Shared<Crystal::TextureComponent> m_HealthBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealthBarBgComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_IndicatorTextureComponent = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;


	Crystal::Timer m_HealthBarShowTimer = {};
	const float m_HealthBarShowTime = 3.0f;
	bool m_bShouldShowHealthBar = false;

	bool m_bIsOnSmashAttack = false;
	Crystal::Timer m_SmashAttackTimer = {};
	const float m_SmashAttackTime = 0.8f;

	float m_Power = 10.0f;


	bool m_bDeathAnimationIsPlaying = false;
public:
	Crystal::Shared<Crystal::PostProcessVolumeActor> GreenTintVolumeActor;
	Crystal::Shared<Crystal::PostProcessVolumeActor> VignetteVolumeActor;
};
