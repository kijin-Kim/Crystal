#pragma once
#include "SpaceCombatTypes.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Core/Logger.h"
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/SpringArmComponent.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/GeometryStaticPipeline.h"

class PlayerShield;

namespace Crystal {
	class TextureComponent;
	class PostProcessVolumeActor;
	class AIPerceptionSourceComponent;
	class PawnMovementComponent;
}

class MyPlayerPawn final : public Crystal::Pawn
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Pawn>(*this);
		ar & *m_MovementComponent;
		ar & *m_CameraComponent;
		ar& m_bShouldFire;
		ar& m_RoundPerMin;
		ar & m_RoundPerSec;
		ar& m_FireInterval;
//		ar& m_FireTimer;
		ar& m_Health;
		ar& m_bHasItem;
	}

public:
	MyPlayerPawn() = default;
	~MyPlayerPawn() override = default;

	void Initialize() override;

	void Begin() override;
	void Update(const float deltaTime) override;
	void SetupInputComponent(Crystal::InputComponent* inputComponent) override;

	void RotateYaw(float value);
	void RotatePitch(float value);
	void MoveForward(float value);
	void MoveRight(float value);
	void MoveUp(float value);
	void RollRight(float value);
	void BeginFire();
	void EndFire();

	void FireMissile();

	void OpenTitleLevel();
	
	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	void UpdateHealth();
	void UpdateItemStatus(ItemType itemType, bool bAcquired);

	void UsePowerItem();
	void UseHealItem();
	void UseShieldItem();

	void ShowDebugCollision();
	void HideDebugCollision();
	void ShowDebugAI();
	void HideDebugAI();

	void OnItemDestroyed(ItemType itemType);

	void SetIsNotInPolluteSphere(bool bIsNotInSphere)
	{
		m_bIsNotInPolluteSphere = bIsNotInSphere;
	}
	bool GetIsNotInPolluteSphere() const { return m_bIsNotInPolluteSphere; }

	STATIC_TYPE_IMPLE(MyPlayerPawn)

private:
	void OnFire();
	void AddDamagedOpacityMultiplier(float opacity);
	void AddHealOpacityMultiplier(float opacity);



private:
	Crystal::Shared<Crystal::StaticMeshComponent> m_StaticMeshComponent = nullptr;
	Crystal::Shared<Crystal::TransformComponent> m_LeftFireSocketComponent = nullptr;
	Crystal::Shared<Crystal::TransformComponent> m_RightFireSocketComponent = nullptr;
	Crystal::Shared<Crystal::PawnMovementComponent> m_MovementComponent = nullptr;
	Crystal::Shared<Crystal::CameraComponent> m_CameraComponent = nullptr;
	Crystal::Shared<Crystal::AIPerceptionSourceComponent> m_AIPerceptionSourceComponent;

	Crystal::Weak<PlayerShield> m_PlayerShield = {};

	bool m_bShouldFire = false;
	float m_RoundPerMin = 1200.0f;
	float m_RoundPerSec = m_RoundPerMin / 60.0f;
	float m_FireInterval = 1.0f / m_RoundPerSec;

	Crystal::Timer m_FireTimer;
	

	const float m_MaxHealth = 100.0f;
	float m_Health = m_MaxHealth;
	
	float m_Power = 5.0f;

	bool m_bHasItem[ItemTypeCount];


	bool m_bIsNotInPolluteSphere = false;

	Crystal::Timer m_HealTimer;
	Crystal::Timer m_HealIntervalTimer;
	const float m_MaxHealTime = 10.0f;
	const float m_HealInterval = 1.0f;
	const float m_HealAmount = 2.0f;
	bool m_bShouldHeal = false;

	bool m_bUseLeftSocekt = true;

public:
	Crystal::Weak<Crystal::PostProcessVolumeActor> DamagedPostProcessActor;
	Crystal::Weak<Crystal::PostProcessVolumeActor> HealPostProcessActor;
	Crystal::Weak<Crystal::PostProcessVolumeActor> ShieldPostProcess;

};
