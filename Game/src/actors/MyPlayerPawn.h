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


namespace Crystal {
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
	
	void OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser) override;

	void UpdateHealth();
	void UpdateItemStatus(ItemType itemType, bool bAcquired);

	void UsePowerItem();
	void UseHealItem();
	void UseShieldItem();

	void ToggleShowDebugCollision();
	void ToggleShowDebugAI();
	


	STATIC_TYPE_IMPLE(MyPlayerPawn)

private:
	void OnFire();
	

private:
	Crystal::Shared<Crystal::PawnMovementComponent> m_MovementComponent = nullptr;
	Crystal::Shared<Crystal::CameraComponent> m_CameraComponent = nullptr;
	Crystal::Shared<Crystal::AIPerceptionSourceComponent> m_AIPerceptionSourceComponent;

	bool m_bShouldFire = false;
	float m_RoundPerMin = 500.0f;
	float m_RoundPerSec = m_RoundPerMin / 60.0f;
	float m_FireInterval = 1.0f / m_RoundPerSec;

	Crystal::Timer m_FireTimer;


	float m_Health = 0.0f;
	float m_MaxHealth = 100.0f;
	
	float m_Power = 1.0f;
	bool m_bIsInVunlnerable = false;

	bool m_bHasItem[ItemTypeCount];

};
