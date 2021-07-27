#pragma once
#include "SpaceWhaleAIController.h"
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


		auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		sphereComponent->SetRadius(804 / 2.0f);
		sphereComponent->SetMass(800000.0f);


		auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		skeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/Biomechanical Whale Animated.fbx"));
		skeletalMeshComponent->PlayAnimation(resourceManager.GetAnimation("assets/models/Biomechanical Whale Animated.fbx"), true);
		skeletalMeshComponent->AddMaterial(material);
		skeletalMeshComponent->SetUnitScale(100.0f);
		skeletalMeshComponent->RotateYaw(180.0f);
		
		
		m_MainComponent = sphereComponent;

		skeletalMeshComponent->AttachTo(m_MainComponent);
	}

	void OnPossessed(Crystal::Weak<Crystal::Controller> controller) override
	{
		Crystal::Pawn::OnPossessed(controller);

		m_MoveTargetLocation = Crystal::Vector3::Zero;
		
		auto spaceWhaleController = Crystal::Cast<SpaceWhaleAIController>(m_Controller);
		spaceWhaleController->MoveToLocation(m_MoveTargetLocation, 300000000.0f, 120.0f);

		auto newFacing = Crystal::Vector3::Normalize(Crystal::Vector3::Subtract(m_MoveTargetLocation, m_MainComponent->GetWorldPosition()));

		auto normalizedForward = Crystal::Vector3::Normalize(m_MainComponent->GetWorldForwardVector());
		auto rotationAxis = Crystal::Vector3::Normalize(Crystal::Vector3::Cross(normalizedForward, newFacing));

		if (Crystal::Vector3::IsZero(rotationAxis))
		{
			return;
		}

		auto angle = Crystal::Vector3::AngleBetweenNormals(normalizedForward, newFacing);
		auto quat = Crystal::Vector4::QuaternionRotationAxis(rotationAxis, angle);

		auto rotation = m_MainComponent->GetRotationQuat();
		auto newQuat = Crystal::Vector4::QuaternionMultiply(rotation, quat);
		m_MainComponent->SetRotationQuat(newQuat);
	}
	
	STATIC_TYPE_IMPLE(SpaceWhale)

private:
	DirectX::XMFLOAT3 m_MoveTargetLocation = Crystal::Vector3::Zero;

};
