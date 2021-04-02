#pragma once

#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Animation.h"

class Kraken : public Crystal::Pawn
{
public:
	Kraken()
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

		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto bodyMaterial = std::make_shared<Crystal::Material>();
		bodyMaterial->SetObjectOwner(resourceManager.GetShader("PBRShader_Skeletal"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);

		bodyMaterial->Set("AlbedoTexture", bodyAlbedoTexture);
		bodyMaterial->Set("RoughnessTexture", bodyRoughnessTexture);
		bodyMaterial->Set("NormalTexture", bodyNormalTexture);

		auto tentacleMaterial = std::make_shared<Crystal::Material>();
		tentacleMaterial->SetObjectOwner(resourceManager.GetShader("PBRShader_Skeletal"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);
		tentacleMaterial->Set("AlbedoTexture", tentacleAlbedoTexture);
		tentacleMaterial->Set("RoughnessTexture", tentacleRoughnessTexture);
		tentacleMaterial->Set("NormalTexture", tentacleNormalTexture);

		std::shared_ptr<Crystal::SkeletalMesh> mesh = std::make_shared<Crystal::SkeletalMesh>("assets/models/KRAKEN.fbx",
			"assets/models/KRAKEN_turn45LeftSmashAttack.fbx");

		auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		skeletalMeshComponent->SetRenderable(mesh);
		skeletalMeshComponent->AddMaterial(bodyMaterial);
		skeletalMeshComponent->AddMaterial(tentacleMaterial);

		m_MainComponent = skeletalMeshComponent;
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
};
