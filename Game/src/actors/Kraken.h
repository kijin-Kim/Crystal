#pragma once

#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Animation.h"

class Kraken : public Crystal::Pawn
{
public:
	Kraken()
	{
		auto& resourceManager = Crystal::ResourceManager::Instance();

	/*	auto bodyMaterial = std::make_shared<Crystal::Material>();
		bodyMaterial->OnCreate();
		bodyMaterial->SetObjectOwner(resourceManager.GetShader("PBRShader_Skeletal"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);
		bodyMaterial->Set("AlbedoTexture", resourceManager.GetTexture("Kraken_Body_Albedo"));
		bodyMaterial->Set("RoughnessTexture", resourceManager.GetTexture("Kraken_Body_Roughness"));
		bodyMaterial->Set("NormalTexture", resourceManager.GetTexture("Kraken_Body_Normal"));
		bodyMaterial->Set("IrradianceTexture", resourceManager.GetTexture("Cube_Skybox_Space_Irradiance"));

		auto tentacleMaterial = std::make_shared<Crystal::Material>();
		tentacleMaterial->OnCreate();
		tentacleMaterial->SetObjectOwner(resourceManager.GetShader("PBRShader_Skeletal"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);
		tentacleMaterial->Set("AlbedoTexture", resourceManager.GetTexture("Kraken_Tentacle_Albedo"));
		tentacleMaterial->Set("RoughnessTexture", resourceManager.GetTexture("Kraken_Tentacle_Roughness"));
		tentacleMaterial->Set("NormalTexture", resourceManager.GetTexture("Kraken_Tentacle_Normal"));
		tentacleMaterial->Set("IrradianceTexture", resourceManager.GetTexture("Cube_Skybox_Space_Irradiance"));*/

		std::shared_ptr<Crystal::SkeletalMesh> mesh = std::make_shared<Crystal::SkeletalMesh>("assets/models/KRAKEN.fbx",
			"assets/models/KRAKEN_idle.fbx");

		auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		skeletalMeshComponent->SetRenderable(mesh);
		skeletalMeshComponent->RotatePitch(90.0f);
		/*skeletalMeshComponent->AddMaterialOld(bodyMaterial);
		skeletalMeshComponent->AddMaterialOld(tentacleMaterial);*/

		m_MainComponent = skeletalMeshComponent;


		SetPosition({ 0.0f, 0.0f, 2000.0f });
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


	STATIC_TYPE_IMPLE(Kraken)
};
