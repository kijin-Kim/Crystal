#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Core/Logger.h"
#include "Crystal/GamePlay/Components/SpringArmComponent.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/LightingStaticPipeline.h"

class TestPawn final : public Crystal::Pawn
{
public:
	TestPawn() = default;
	~TestPawn() override = default;

	void Initialize() override;

	

	void SetupInputComponent(Crystal::InputComponent* inputComponent) override;

	void RotateYaw(float value);
	void RotatePitch(float value);
	void MoveForward(float value);
	void MoveRight(float value);
	void MoveUp(float value);
	void RollRight(float value);
	void BeginFire();

	STATIC_TYPE_IMPLE(TestPawn)

private:
	std::shared_ptr<Crystal::MovementComponent> m_MovementComponent = nullptr;
	std::shared_ptr<Crystal::CameraComponent> m_CameraComponent = nullptr;
};
