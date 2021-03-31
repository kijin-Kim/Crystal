#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Core/Logger.h"
#include "Crystal/GamePlay/Components/SpringArmComponent.h"
#include "Crystal/Resources/ResourceManager.h"

class TestPawn final : public Crystal::Pawn
{
public:
	TestPawn()
	{
		////// TEMPORARY ////

		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto pbrMaterial = std::make_shared<Crystal::Material>(resourceManager.GetShader("PBRShader_Static"));
		pbrMaterial->Set("AlbedoTexture", resourceManager.GetTexture("Frigate_Albedo"));
		pbrMaterial->Set("MetallicTexture", resourceManager.GetTexture("Frigate_Metallic"));
		pbrMaterial->Set("RoughnessTexture", resourceManager.GetTexture("Frigate_Roughness"));
		pbrMaterial->Set("NormalTexture", resourceManager.GetTexture("Frigate_Normal"));
		//===================================================================================================

		auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		boundingOrientedBoxComponent->SetExtents({ 9080.0f / 2.0f, 2940.0f / 2.0f, 8690.0f / 2.0f });

		m_MainComponent = boundingOrientedBoxComponent;

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable("Frigate"));
		staticMeshComponent->SetMaterial(pbrMaterial);
		staticMeshComponent->SetLocalPosition({ 0.0f, 0.0f, 596.0f });
		staticMeshComponent->SetAttachment(m_MainComponent);

		auto springArmComponent = CreateComponent<Crystal::SpringArmComponent>("SpringArmComponent");
		springArmComponent->SetOffsetPosition({ 0, 4500.0f, -15000.0f });
		springArmComponent->SetAttachment(m_MainComponent);

		m_CameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		m_CameraComponent->SetFieldOfView(60.0f);
		m_CameraComponent->SetNearPlane(1000.0f);
		m_CameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		m_CameraComponent->SetFarPlane(10000000.0f);
		m_CameraComponent->SetAttachment(springArmComponent);

		m_MovementComponent = CreateComponent<Crystal::MovementComponent>("MovementComponent");
		m_MovementComponent->SetTargetComponent(m_MainComponent);
		
		Crystal::ApplicationUtility::GetPlayerController().SetMainCamera(m_CameraComponent);
	}

	~TestPawn() override = default;

	void Begin() override
	{
		Pawn::Begin();
		CS_DEBUG_INFO("Test Pawn Begin");
	}

	void End() override
	{
		Pawn::End();
		CS_DEBUG_INFO("Test Pawn End");
	}

	void Update(const float deltaTime) override
	{
		Pawn::Update(deltaTime); // Updating All Component
	}

	void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
		Pawn::SetupInputComponent(inputComponent);

		inputComponent->BindAxis("MoveForward", CS_AXIS_FN(TestPawn::MoveForward));
		inputComponent->BindAxis("MoveRight", CS_AXIS_FN(TestPawn::MoveRight));
		inputComponent->BindAxis("MoveUp", CS_AXIS_FN(TestPawn::MoveUp));
		inputComponent->BindAxis("RollRight", CS_AXIS_FN(TestPawn::RollRight));

		inputComponent->BindAxis("LookUp", CS_AXIS_FN(TestPawn::RotatePitch));
		inputComponent->BindAxis("Turn", CS_AXIS_FN(TestPawn::RotateYaw));

		inputComponent->BindAction("Fire", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(TestPawn::BeginFire));
	}

	void RotateYaw(float value)
	{
		value *= 0.05f;
		m_MainComponent->RotateYaw(value);
	}

	void RotatePitch(float value)
	{
		value *= 0.05f;
		m_MainComponent->RotatePitch(value);
	}

	void MoveForward(float value)
	{
		value *= 1300.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalForwardVector(), value);
		m_MovementComponent->AddForce(force);
	}

	void MoveRight(float value)
	{
		value *= 1300.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalRightVector(), value);
		m_MovementComponent->AddForce(force);
	}

	void MoveUp(float value)
	{
		value *= 1300.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalUpVector(), value);
		m_MovementComponent->AddForce(force);
	}

	void RollRight(float value)
	{
		m_MainComponent->RotateRoll(-value);
	}

	void BeginFire()
	{
		CS_DEBUG_INFO("BeginFire!!");
		const auto start = m_CameraComponent->GetWorldPosition();
		const auto direction = m_CameraComponent->GetWorldForwardVector();
		const float maxDistance = 1000000.0f;

		auto level = Crystal::Cast<Crystal::Level>(GetObjectOwner(ObjectOwnerType::OOT_Level));
		if (level)
			level->DrawDebugLine(start, direction, maxDistance, Crystal::Vector3::Green);
	}

private:
	Crystal::MovementComponent* m_MovementComponent = nullptr;
	Crystal::CameraComponent* m_CameraComponent = nullptr;
};
