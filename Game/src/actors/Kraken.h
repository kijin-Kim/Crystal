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

		sphereComponent->SetInverseMass(0.0f);


		m_SkeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		m_SkeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/KRAKEN.fbx"));
		m_SkeletalMeshComponent->PlayAnimationWithEvent(resourceManager.GetAnimation("assets/models/KRAKEN_idle.fbx"), true, CS_ANIMATION_FN(Kraken::OnAttack), 3.0f);
		m_SkeletalMeshComponent->RotatePitch(90.0f);
		m_SkeletalMeshComponent->AddMaterial(std::move(bodyMaterial));
		m_SkeletalMeshComponent->AddMaterial(std::move(tentacleMaterial));
		m_SkeletalMeshComponent->SetLocalPosition({0.0f, -400.0f, 0.0f});


		m_MainComponent = sphereComponent;

		m_SkeletalMeshComponent->AttachTo(m_MainComponent);

		
	}

	void OnAttack()
	{
		auto& resourceManager = Crystal::ResourceManager::Instance();
		m_SkeletalMeshComponent->PlayAnimationWithEndEvent(resourceManager.GetAnimation("assets/models/KRAKEN_smashAttack.fbx"), false, CS_ANIMATION_FN(Kraken::OnIdle));
	}

	void OnIdle()
	{
		auto& resourceManager = Crystal::ResourceManager::Instance();
		m_SkeletalMeshComponent->PlayAnimationWithEvent(resourceManager.GetAnimation("assets/models/KRAKEN_idle.fbx"), true, CS_ANIMATION_FN(Kraken::OnAttack), 3.0f);
	}


	STATIC_TYPE_IMPLE(Kraken)

private:
	Crystal::Shared<Crystal::SkeletalMeshComponent> m_SkeletalMeshComponent = nullptr;
};

