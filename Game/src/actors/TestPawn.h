#pragma once
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Core/Logger.h"

class TestPawn final : public Crystal::Pawn
{
public:
	TestPawn()
	{
		////// TEMPORARY ////


		/*텍스쳐를 만듭니다.*/
		auto albedoTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga");
		auto metallicTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga");
		auto roughnessTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga");
		auto normalTexture = std::make_shared<Crystal::Texture>("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga");
		
		albedoTexture->CreateShaderResourceView(albedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		metallicTexture->CreateShaderResourceView(metallicTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		roughnessTexture->CreateShaderResourceView(roughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		normalTexture->CreateShaderResourceView(normalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


		/*Material을 생성합니다.*/
		auto& shaderManager = Crystal::ShaderManager::Instance();
		auto pbrMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader_Static"));

		pbrMaterial->Set("AlbedoTexture", albedoTexture);
		pbrMaterial->Set("MetallicTexture", metallicTexture);
		pbrMaterial->Set("RoughnessTexture", roughnessTexture);
		pbrMaterial->Set("NormalTexture", normalTexture);

		auto* mesh = new Crystal::StaticMesh("assets/models/SM_Frigate_BE2.fbx");	
		mesh->SetMaterial(pbrMaterial, 0);



		
		m_MeshComponent = CreateComponent<Crystal::StaticMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);
		
		m_MainComponent = m_MeshComponent;


		m_FollowMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("FollowMeshComponent");
		m_FollowMeshComponent->SetRenderable(mesh);
		m_FollowMeshComponent->SetPosition(DirectX::XMFLOAT3(0.0f, 5000.0f, 0.0f));
		m_FollowMeshComponent->AttachToComponent(m_MainComponent);
		

		//m_CameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		//m_CameraComponent->AttachToComponent(m_MainComponent);
		//m_CameraComponent->SetPosition(DirectX::XMFLOAT3(0, 4500.0f, -15000.0f));
		//m_CameraComponent->RotatePitch(15.0f);
		//m_CameraComponent->SetFieldOfView(60.0f);
		//m_CameraComponent->SetNearPlane(1.0f);
		//m_CameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		//m_CameraComponent->SetFarPlane(100000.0f);

//		Crystal::ApplicationUtility::GetPlayerController().SetMainCamera(m_CameraComponent);

	
		
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
		auto position = m_MeshComponent->GetPosition();
		position.x += deltaTime * 1000.0f;

		m_MeshComponent->RotateRoll(deltaTime * 10.0f);
		m_MeshComponent->SetPosition(position);

		
		
		
	}

	void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
	}
private:
	Crystal::CameraComponent* m_CameraComponent = nullptr;
	Crystal::MeshComponent* m_FollowMeshComponent = nullptr;
};
