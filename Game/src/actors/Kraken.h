#pragma once

#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

class Kraken : public Crystal::Pawn
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Pawn>(*this);
	}

public:
	void Initialize() override
	{
		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto bodyMaterial = std::make_unique<Crystal::Material>();
		bodyMaterial->AlbedoTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_BODY_BaseColor.tga");
		bodyMaterial->RoughnessTexture = resourceManager.GetTexture("assets/textures/Kraken/T_M_KRAKEN_Mat_KRAKEN_MAIN_BODY_Roughness.tga");
		bodyMaterial->NormalTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_BODY_NRM.tga");
		bodyMaterial->ShadingModel = Crystal::EShadingModel::SM_Lit;
		bodyMaterial->BlendMode = Crystal::EBlendMode::BM_Opaque;

		auto tentacleMaterial = std::make_unique<Crystal::Material>();
		tentacleMaterial->AlbedoTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_BaseColor.tga");
		tentacleMaterial->RoughnessTexture = resourceManager.GetTexture("assets/textures/Kraken/T_M_KRAKEN_Mat_TENTACLES_LEGS_CLAWS_Roughness.tga");
		tentacleMaterial->NormalTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_CLAW_NRM.tga");
		tentacleMaterial->ShadingModel = Crystal::EShadingModel::SM_Lit;
		tentacleMaterial->BlendMode = Crystal::EBlendMode::BM_Opaque;


		auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		sphereComponent->SetRadius(804 / 2.0f);

		sphereComponent->SetMass(40000.0f);


		
		auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		skeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/KRAKEN.fbx"));
		skeletalMeshComponent->SetAnimation(resourceManager.GetAnimation("assets/models/KRAKEN_idle.fbx"));
		skeletalMeshComponent->RotatePitch(90.0f);
		skeletalMeshComponent->AddMaterial(std::move(bodyMaterial));
		skeletalMeshComponent->AddMaterial(std::move(tentacleMaterial));
		skeletalMeshComponent->SetLocalPosition({0.0f, -400.0f, 0.0f});


		m_MainComponent = sphereComponent;

		skeletalMeshComponent->AttachTo(m_MainComponent);


		SetPosition({0.0f, 0.0f, 2000.0f});
	}


	STATIC_TYPE_IMPLE(Kraken)
};
