#pragma once
#include "SpaceWhaleAIController.h"
#include "Crystal/GamePlay/AI/Blackboard.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/Resources/ResourceManager.h"

class SpaceWhale : public Crystal::Pawn
{

public:
	void Initialize() override
	{
		Crystal::Pawn::Initialize();
		
		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto material = Crystal::CreateShared<Crystal::Material>();
		material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Whale Diffuse.jpg");
		material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Whale Roughness.jpg");
		material->NormalTexture = resourceManager.GetTexture("assets/textures/Whale Normals.jpg");
		material->EmissiveTexture = resourceManager.GetTexture("assets/textures/Whale Emissive.jpg");
		material->ShadingModel = Crystal::EShadingModel::SM_Lit;
		material->BlendMode = Crystal::EBlendMode::BM_Opaque;


		/*auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		sphereComponent->SetRadius(804 / 2.0f);
		sphereComponent->SetMass(800000.0f);*/

		auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
		boundingOrientedBoxComponent->SetExtents({ 4.9f / 2.0f , 2.8f / 2.0f, 11.0f / 2.0f });
		boundingOrientedBoxComponent->SetUnitScale(100.0f);


		auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		skeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/Biomechanical Whale Animated.fbx"));
		skeletalMeshComponent->PlayAnimation(resourceManager.GetAnimation("assets/models/Biomechanical Whale Animated.fbx"), true);
		skeletalMeshComponent->AddMaterial(material);
		skeletalMeshComponent->RotateYaw(180.0f);
		
		
		m_MainComponent = boundingOrientedBoxComponent;

		skeletalMeshComponent->AttachTo(m_MainComponent);
	}

	void OnPossessed(Crystal::Weak<Crystal::Controller> controller) override
	{
		
		Crystal::Pawn::OnPossessed(controller);

	
		auto spaceWhaleController = Crystal::Cast<SpaceWhaleAIController>(m_Controller);
		
		spaceWhaleController->GetBlackboardComponent()->SetValueAsFloat3("TargetLocation", { -5000.0f , 0.0f, 5000.0f });
		

	}
	
	STATIC_TYPE_IMPLE(SpaceWhale)

};
