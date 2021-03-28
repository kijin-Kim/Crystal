#pragma once
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Core/Logger.h"
#include "Crystal/GamePlay/Components/SpringArmComponent.h"

class TestPawn final : public Crystal::Pawn
{
public:
	TestPawn(Crystal::Object* parent) : Crystal::Pawn(parent)
	{
		////// TEMPORARY ////


		auto albedoTexture 
			= std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga");
		auto metallicTexture 
			= std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga");
		auto roughnessTexture 
			= std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga");
		auto normalTexture 
			= std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga");

		albedoTexture->CreateShaderResourceView(albedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		metallicTexture->CreateShaderResourceView(metallicTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		roughnessTexture->CreateShaderResourceView(roughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		normalTexture->CreateShaderResourceView(normalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


		auto& shaderManager = Crystal::ShaderManager::Instance();
		auto pbrMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader_Static"));

		pbrMaterial->Set("AlbedoTexture", albedoTexture);
		pbrMaterial->Set("MetallicTexture", metallicTexture);
		pbrMaterial->Set("RoughnessTexture", roughnessTexture);
		pbrMaterial->Set("NormalTexture", normalTexture);

		auto* mesh = new Crystal::StaticMesh("assets/models/SM_Frigate_BE2.fbx");
		mesh->SetMaterial(pbrMaterial, 0);
		// ========================================================================
		

		auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		boundingOrientedBoxComponent->SetExtents({ 9080.0f / 2.0f, 2940.0f / 2.0f, 8690.0f / 2.0f });

		m_MainComponent = boundingOrientedBoxComponent;


		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		staticMeshComponent->SetRenderable(mesh);
		staticMeshComponent->SetLocalPosition({ 0.0f, 0.0f, 596.0f });
		staticMeshComponent->SetAttachment(m_MainComponent);


		auto springArmComponent = CreateComponent<Crystal::SpringArmComponent>("SpringArmComponent");
		springArmComponent->RotatePitch(15.0f);
		springArmComponent->SetLocalPosition({ 0, 4500.0f, -15000.0f });
		springArmComponent->SetAttachment(m_MainComponent);
		
		auto cameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		//cameraComponent->SetLocalPosition(DirectX::XMFLOAT3(0, 4500.0f, -15000.0f));
		//cameraComponent->RotatePitch(15.0f);
		cameraComponent->SetFieldOfView(60.0f);
		cameraComponent->SetNearPlane(1000.0f);
		cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		cameraComponent->SetFarPlane(10000000.0f);
		cameraComponent->SetAttachment(springArmComponent);

		
		m_MovementComponent = CreateComponent<Crystal::MovementComponent>("MovementComponent");
		m_MovementComponent->SetTargetComponent(m_MainComponent);


		Crystal::ApplicationUtility::GetPlayerController().SetMainCamera(cameraComponent);
	
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
		value *= 2000.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetForward(), value);
		m_MovementComponent->AddForce(force);
	}

	void MoveRight(float value)
	{	
		value *= 2000.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetRight(), value);
		m_MovementComponent->AddForce(force);
	}

	void MoveUp(float value)
	{
		value *= 2000.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetUp(), value);
		m_MovementComponent->AddForce(force);
	}

	void RollRight(float value)
	{
		m_MainComponent->RotateRoll(-value);
	}

	void BeginFire()
	{
		CS_DEBUG_INFO("BeginFire!!");
		m_MainComponent->SetVelocity(Crystal::Vector3::Zero);
		auto start = m_MainComponent->GetWorldPosition();

		Crystal::Level* level = (Crystal::Level*)GetParentObject();
		level->DrawDebugLine(start, m_MainComponent->GetForward(), 1000000.0f, { 1.0f, 1.0f, 1.0f });
	}

private:
	Crystal::MovementComponent* m_MovementComponent = nullptr;
};
