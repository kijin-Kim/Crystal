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

		auto* boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		boundingOrientedBoxComponent->SetExtents({ 9080.0f / 2.0f, 2940.0f / 2.0f, 8690.0f / 2.0f});
		m_MainComponent = boundingOrientedBoxComponent;
		//m_MainComponent->SetPosition({ 0.0f, 0.0f, 50000.0f });

		m_MeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);
		m_MeshComponent->SetPosition({ 0.0f, 0.0f, 596.0f });
		m_MeshComponent->AttachToComponent(m_MainComponent);

		m_CameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		m_CameraComponent->SetPosition(DirectX::XMFLOAT3(0, 4500.0f, -15000.0f));
		m_CameraComponent->RotatePitch(15.0f);
		m_CameraComponent->SetFieldOfView(60.0f);
		m_CameraComponent->SetNearPlane(1000.0f);
		m_CameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		m_CameraComponent->SetFarPlane(10000000.0f);
		m_CameraComponent->AttachToComponent(m_MainComponent);

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

		inputComponent->BindAxis("LookUp", CS_AXIS_FN(TestPawn::RotatePitch));
		inputComponent->BindAxis("Turn", CS_AXIS_FN(TestPawn::RotateYaw));

		inputComponent->BindAction("Fire", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(TestPawn::BeginFire));

	}

	void RotateYaw(float value)
	{
		//ApplicationUtility::GetPlayerController().ProcessYawInput(DirectX::XMConvertToRadians(value));
		const float valueScale = 0.1f;
		value *= valueScale;
		m_MainComponent->RotateYaw(value);
	}

	void RotatePitch(float value)
	{
		//ApplicationUtility::GetPlayerController().ProcessPitchInput(DirectX::XMConvertToRadians(value));
		const float valueScale = 0.1f;
		value *= valueScale;
		m_MainComponent->RotatePitch(-value);
	}

	void MoveForward(float value)
	{
		value *= 1000.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetForward(), { value, value, value });
		m_MovementComponent->AddForce(force);
	}

	void MoveRight(float value)
	{
		value *= 1000.0f;
		DirectX::XMFLOAT3 force = Crystal::Vector3::Multiply(m_MainComponent->GetRight(), { value, value, value });
		m_MovementComponent->AddForce(force);
	}

	void BeginFire()
	{
		CS_DEBUG_INFO("BeginFire!!");
		auto start = m_MainComponent->GetPosition();
		DirectX::XMFLOAT3 maxDistance = { 100000.0f, 100000.0f, 100000.0f };
		auto end = Crystal::Vector3::Add(start, Crystal::Vector3::Multiply(m_MainComponent->GetForward(), maxDistance));

		Crystal::Level* level = (Crystal::Level*)GetParent();
		level->DrawDebugLine(start, end);
	}

private:
	Crystal::CameraComponent* m_CameraComponent = nullptr;
};
