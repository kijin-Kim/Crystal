#pragma once

#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Animation.h"

class Kraken : public Crystal::Pawn
{
public:
	Kraken(Crystal::Object* parent) : Crystal::Pawn(parent)
	{
		////// TEMPORARY ////
		auto bodyAlbedoTexture 
			= std::make_shared<Crystal::Texture>("assets/textures/Kraken/Tex_KRAKEN_BODY_BaseColor.tga");
		auto bodyRoughnessTexture 
			= std::make_shared<Crystal::Texture>("assets/textures/Kraken/T_M_KRAKEN_Mat_KRAKEN_MAIN_BODY_Roughness.tga");
		auto bodyNormalTexture = std::make_shared<Crystal::Texture>("assets/textures/Kraken/Tex_KRAKEN_BODY_NRM.tga");
		bodyAlbedoTexture->CreateShaderResourceView(bodyAlbedoTexture->GetResource()->GetDesc().Format, 
			D3D12_SRV_DIMENSION_TEXTURE2D);
		bodyRoughnessTexture->CreateShaderResourceView(bodyRoughnessTexture->GetResource()->GetDesc().Format, 
			D3D12_SRV_DIMENSION_TEXTURE2D);
		bodyNormalTexture->CreateShaderResourceView(bodyNormalTexture->GetResource()->GetDesc().Format, 
			D3D12_SRV_DIMENSION_TEXTURE2D);
		

		auto tentacleAlbedoTexture 
			= std::make_shared<Crystal::Texture>("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_BaseColor.tga");
		auto tentacleRoughnessTexture
			= std::make_shared<Crystal::Texture>("assets/textures/Kraken/T_M_KRAKEN_Mat_TENTACLES_LEGS_CLAWS_Roughness.tga");
		auto tentacleNormalTexture 
			= std::make_shared<Crystal::Texture>("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_CLAW_NRM.tga");
		tentacleAlbedoTexture->CreateShaderResourceView(tentacleAlbedoTexture->GetResource()->GetDesc().Format, 
			D3D12_SRV_DIMENSION_TEXTURE2D);
		tentacleRoughnessTexture->CreateShaderResourceView(tentacleRoughnessTexture->GetResource()->GetDesc().Format, 
			D3D12_SRV_DIMENSION_TEXTURE2D);
		tentacleNormalTexture->CreateShaderResourceView(tentacleNormalTexture->GetResource()->GetDesc().Format, 
			D3D12_SRV_DIMENSION_TEXTURE2D);


		auto& shaderManager = Crystal::ShaderManager::Instance();

		auto bodyMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader_Skeletal"));
		bodyMaterial->Set("AlbedoTexture", bodyAlbedoTexture);
		bodyMaterial->Set("RoughnessTexture", bodyRoughnessTexture);
		bodyMaterial->Set("NormalTexture", bodyNormalTexture);

		auto tentacleMaterial = std::make_shared<Crystal::Material>(shaderManager.GetShader("PBRShader_Skeletal"));
		tentacleMaterial->Set("AlbedoTexture", tentacleAlbedoTexture);
		tentacleMaterial->Set("RoughnessTexture", tentacleRoughnessTexture);
		tentacleMaterial->Set("NormalTexture", tentacleNormalTexture);

		Crystal::SkeletalMesh* mesh = new Crystal::SkeletalMesh("assets/models/KRAKEN.fbx", 
			"assets/models/KRAKEN_turn45LeftSmashAttack.fbx");
		mesh->SetMaterial(bodyMaterial);
		mesh->SetMaterial(tentacleMaterial, 1);

		m_MeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		m_MeshComponent->SetRenderable(mesh);


		m_MainComponent = m_MeshComponent;

	
		m_MeshComponent->AddPitch(90.0f);
		m_MeshComponent->SetLocalPosition({ -5000.0f, 0.0f, 0.0f });
		m_MeshComponent->SetScale(10.0f);

	}

	virtual ~Kraken()
	{
	}

	void Begin() override
	{
		Pawn::Begin();
	}

	void End() override
	{
		Pawn::End();
	}

	void Update(const float deltaTime) override
	{
		Pawn::Update(deltaTime); // Updating All Component
	}

	void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
	}
private:
	Crystal::BoundingSphereComponent* m_BoundingSphereComponent = nullptr;
};

