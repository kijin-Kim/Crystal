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
	TestPawn()
	{
		////// TEMPORARY ////

		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto pbrMaterial = std::make_shared<Crystal::Material>();
		pbrMaterial->OnCreate();
		pbrMaterial->SetObjectOwner(resourceManager.GetShader("PBRShader_Static"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);
		pbrMaterial->Set("AlbedoTexture", resourceManager.GetTexture("Frigate_Albedo"));
		pbrMaterial->Set("MetallicTexture", resourceManager.GetTexture("Frigate_Metallic"));
		pbrMaterial->Set("RoughnessTexture", resourceManager.GetTexture("Frigate_Roughness"));
		pbrMaterial->Set("NormalTexture", resourceManager.GetTexture("Frigate_Normal"));
		pbrMaterial->Set("IrradianceTexture", resourceManager.GetTexture("Cube_Skybox_Space_Irradiance"));

		auto material = std::make_unique<Crystal::LightingStaticPipeline::Material>();
		material->AlbedoTexture = resourceManager.GetTexture("Frigate_Albedo");
		material->MetallicTexture = resourceManager.GetTexture("Frigate_Metallic");
		material->RoughnessTexture = resourceManager.GetTexture("Frigate_Roughness");
		material->NormalTexture = resourceManager.GetTexture("Frigate_Normal");


		

		//===================================================================================================

		/*auto boxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		boxComponent->SetExtents({ 908.0f / 2.0f, 294.0f / 2.0f, 869.0f / 2.0f });*/

		auto boxComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingOrientedBoxComponent");
		boxComponent->SetRadius(908.0f / 2.0f);
		boxComponent->SetMass(20000.0f);
		//boxComponent->SetExtents({ 908.0f / 2.0f, 294.0f / 2.0f, 869.0f / 2.0f });

		m_MainComponent = boxComponent;

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable("Frigate"));
		staticMeshComponent->AddMaterialOld(pbrMaterial);
		staticMeshComponent->AddMaterial(std::move(material));
		SetAttachment(staticMeshComponent, m_MainComponent);

		auto springArmComponent = CreateComponent<Crystal::SpringArmComponent>("SpringArmComponent");
		springArmComponent->SetOffsetPosition({ 0, 450.0f, -1500.0f });
		SetAttachment(springArmComponent, m_MainComponent);

		m_CameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		m_CameraComponent->SetFieldOfView(60.0f);
		m_CameraComponent->SetNearPlane(100.0f);
		m_CameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		m_CameraComponent->SetFarPlane(1000000.0f);
		SetAttachment(m_CameraComponent, springArmComponent);

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
		value *= 30000000;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalForwardVector(), value);
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->AddForce(force);
	}

	void MoveRight(float value)
	{
		value *= 30000000;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalRightVector(), value);
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->AddForce(force);
	}

	void MoveUp(float value)
	{
		value *= 30000000;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetLocalUpVector(), value);
		Crystal::Cast<Crystal::BoundingSphereComponent>(m_MainComponent)->AddForce(force);
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
		const float maxDistance = 10000.0f;

		auto level = Crystal::Cast<Crystal::Level>(GetObjectOwner(Actor::ActorOwnerType::Owner_Level));
		if (level)
			level->DrawDebugLine(start, direction, maxDistance, Crystal::Vector3::Green);
	}

private:
	std::shared_ptr<Crystal::MovementComponent> m_MovementComponent = nullptr;
	std::shared_ptr<Crystal::CameraComponent> m_CameraComponent = nullptr;
};
