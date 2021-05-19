#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Core/Logger.h"
#include "Crystal/Core/Timer.h"
#include "Crystal/GamePlay/Components/SpringArmComponent.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/GeometryStaticPipeline.h"


class TestPawn final : public Crystal::Pawn
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Pawn>(*this);
		ar & *m_MovementComponent;
		ar & *m_CameraComponent;
	}

public:
	TestPawn() = default;
	~TestPawn() override = default;

	void Initialize() override;

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


	STATIC_TYPE_IMPLE(TestPawn)

private:
	void OnFire();

	
	

private:
	std::shared_ptr<Crystal::MovementComponent> m_MovementComponent = nullptr;
	std::shared_ptr<Crystal::CameraComponent> m_CameraComponent = nullptr;

	bool m_bShouldFire = false;
	float m_RoundPerMin = 500.0f;
	float m_RoundPerSec = m_RoundPerMin / 60.0f;
	float m_FireInterval = 1.0f / m_RoundPerSec;
	

	Crystal::Timer m_FireTimer;
};
