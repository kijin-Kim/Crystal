#pragma once

#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Animation.h"

class Kraken : public Crystal::Pawn
{
public:
	Kraken()
	{
		////// TEMPORARY ////


		/*텍스쳐를 만듭니다.*/
		std::shared_ptr<Crystal::Texture> bodyAlbedoTexture = std::make_shared<Crystal::Texture>("assets/textures/Kraken/Tex_KRAKEN_BODY_BaseColor.tga");
		std::shared_ptr<Crystal::Texture> bodyRoughnessTexture = std::make_shared<Crystal::Texture>("assets/textures/Kraken/T_M_KRAKEN_Mat_KRAKEN_MAIN_BODY_Roughness.tga");
		std::shared_ptr<Crystal::Texture> bodyNormalTexture = std::make_shared<Crystal::Texture>("assets/textures/Kraken/Tex_KRAKEN_BODY_NRM.tga");
		bodyAlbedoTexture->CreateShaderResourceView(bodyAlbedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		bodyRoughnessTexture->CreateShaderResourceView(bodyRoughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		bodyNormalTexture->CreateShaderResourceView(bodyNormalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		

		std::shared_ptr<Crystal::Texture> tentacleAlbedoTexture = std::make_shared<Crystal::Texture>("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_BaseColor.tga");
		std::shared_ptr<Crystal::Texture> tentacleRoughnessTexture = std::make_shared<Crystal::Texture>("assets/textures/Kraken/T_M_KRAKEN_Mat_TENTACLES_LEGS_CLAWS_Roughness.tga");
		std::shared_ptr<Crystal::Texture> tentacleNormalTexture = std::make_shared<Crystal::Texture>("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_CLAW_NRM.tga");
		tentacleAlbedoTexture->CreateShaderResourceView(tentacleAlbedoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		tentacleRoughnessTexture->CreateShaderResourceView(tentacleRoughnessTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		tentacleNormalTexture->CreateShaderResourceView(tentacleNormalTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);


		/*Material을 생성합니다.*/
		auto& shaderManager = Crystal::ShaderManager::Instance();

		std::shared_ptr<Crystal::Material> bodyMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader_Skeletal"));
		bodyMaterial->Set("AlbedoTexture", bodyAlbedoTexture);
		bodyMaterial->Set("RoughnessTexture", bodyRoughnessTexture);
		bodyMaterial->Set("NormalTexture", bodyNormalTexture);

		std::shared_ptr<Crystal::Material> tentacleMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader_Skeletal"));
		tentacleMaterial->Set("AlbedoTexture", tentacleAlbedoTexture);
		tentacleMaterial->Set("RoughnessTexture", tentacleRoughnessTexture);
		tentacleMaterial->Set("NormalTexture", tentacleNormalTexture);

		Crystal::SkeletalMesh* mesh = new Crystal::SkeletalMesh("assets/models/KRAKEN.fbx", "assets/models/KRAKEN_turn45LeftSmashAttack.fbx");
		mesh->SetMaterial(bodyMaterial);
		mesh->SetMaterial(tentacleMaterial, 1);

		m_MeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);

		m_MainComponent = m_MeshComponent;

	
		m_MeshComponent->RotatePitch(90.0f);
		m_MeshComponent->SetPosition({ -5000.0f, 0.0f, 0.0f });
		m_MeshComponent->SetScale(2.0f);

	}

	virtual ~Kraken()
	{
	}

	virtual void Begin() override
	{
		Pawn::Begin();
	}

	virtual void End() override
	{
		Pawn::End();
	}

	virtual void Update(float deltaTime) override
	{
		Pawn::Update(deltaTime); // Updating All Component
	}

	virtual void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
	}
};

