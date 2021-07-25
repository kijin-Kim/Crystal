#pragma once
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/Resources/ResourceManager.h"

class SpaceWhale : public Crystal::Pawn
{

public:
	void Initialize() override
	{

		Pawn::Initialize();
		
		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto material = Crystal::CreateShared<Crystal::Material>();
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Whale Diffuse.jpg");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Whale Roughness.jpg");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Whale Normals.jpg");
		material->EmissiveTexture = resourceManager.GetTexture("assets/textures/Whale Emissive.jpg");
		material->ShadingModel = Crystal::EShadingModel::SM_Lit;
		material->BlendMode = Crystal::EBlendMode::BM_Opaque;


		auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		sphereComponent->SetRadius(804 / 2.0f);

		sphereComponent->SetMass(40000.0f);
		

		

		auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		skeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/Biomechanical Whale Animated.fbx"));
		skeletalMeshComponent->PlayAnimation(resourceManager.GetAnimation("assets/models/Biomechanical Whale Animated.fbx"), true);
		skeletalMeshComponent->AddMaterial(material);
		skeletalMeshComponent->SetUnitScale(100.0f);
		
		m_MainComponent = sphereComponent;

		skeletalMeshComponent->AttachTo(m_MainComponent);


	}

	STATIC_TYPE_IMPLE(SpaceWhale)

};
