#pragma once
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Core/Logger.h"

class TestPawn final : public Crystal::Pawn
{
public:
	TestPawn(Crystal::Object* parent) : Crystal::Pawn(parent)
	{
		////// TEMPORARY ////


		auto albedoTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga");
		auto metallicTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga");
		auto roughnessTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga");
		auto normalTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga");

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
		//auto* mesh = new Crystal::StaticMesh("assets/models/SM_Dreadnought_MB1.fbx");
		mesh->SetMaterial(pbrMaterial, 0);
		// ========================================================================

		/*auto* boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		boundingOrientedBoxComponent->SetExtents({ 9080.0f / 2.0f, 2940.0f / 2.0f, 8690.0f / 2.0f });
		m_MainComponent = boundingOrientedBoxComponent;*/



		m_MeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);
		m_MeshComponent->SetLocalPosition({ 0.0f, 0.0f, 596.0f });
		//m_MeshComponent->AttachToComponent(m_MainComponent);
		m_MainComponent = m_MeshComponent;

		m_MovementComponent = CreateComponent<Crystal::MovementComponent>("MovementComponent");
		m_MovementComponent->SetTargetComponent(m_MainComponent);

		m_TempMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		m_TempMeshComponent->SetRenderable(mesh);
		m_TempMeshComponent->SetLocalPosition({ -5000.0f, 5000.0f, 596.0f });
		m_TempMeshComponent->SetAttachment(m_MeshComponent);
		
		m_CameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		m_CameraComponent->SetLocalPosition(DirectX::XMFLOAT3(0, 4500.0f, -15000.0f));
		m_CameraComponent->AddPitch(15.0f);
		m_CameraComponent->SetFieldOfView(60.0f);
		m_CameraComponent->SetNearPlane(1000.0f);
		m_CameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		m_CameraComponent->SetFarPlane(10000000.0f);
		m_CameraComponent->SetAttachment(m_MainComponent);


		
		m_MainComponent->AddYaw(90.0f - 14.0f);

		//Crystal::ApplicationUtility::GetPlayerController().SetMainCamera(m_CameraComponent);
	
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

		inputComponent->BindAxis("LookUp", CS_AXIS_FN(TestPawn::AddPitch));
		inputComponent->BindAxis("Turn", CS_AXIS_FN(TestPawn::AddYaw));



		inputComponent->BindAction("Fire", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(TestPawn::BeginFire));
	}

	void AddYaw(float value)
	{
		value *= 0.1f;
		m_MainComponent->AddYaw(value);

		// 축이 돌아가지만 여전히 같은 축에 곱하는중 
		
	}

	void AddPitch(float value)
	{
		value *= 0.1f;
		//m_MainComponent->AddPitch(value);
	}

	void MoveForward(float value)
	{
		value *= 10000.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetForward(), value);
		m_MovementComponent->AddForce(force);
	}

	void MoveRight(float value)
	{	
		value *= 1000.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetRight(), value);
		m_MovementComponent->AddForce(force);
	}

	void MoveUp(float value)
	{
		value *= 1000.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetUp(), value);
		m_MovementComponent->AddForce(force);
	}

	void RollRight(float value)
	{
		m_MainComponent->AddRoll(-value);
	}

	void BeginFire()
	{
		m_MainComponent->SetVelocity(Crystal::Vector3::Zero);
		CS_DEBUG_INFO("BeginFire!!");
		auto start = m_MainComponent->GetWorldPosition();
		DirectX::XMFLOAT3 maxDistance = { 1000000.0f, 1000000.0f, 1000000.0f };
		auto end = Crystal::Vector3::Add(start, Crystal::Vector3::Multiply(m_MainComponent->GetForward(), maxDistance));

		Crystal::Level* level = (Crystal::Level*)GetParent();
		level->DrawDebugLine(start, end);
	}

private:
	Crystal::StaticMeshComponent* m_TempMeshComponent = nullptr;
	Crystal::CameraComponent* m_CameraComponent = nullptr;

};
