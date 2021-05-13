#pragma once

#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Animation.h"
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

		auto bodyMaterial = std::make_unique<Crystal::Material>();
		bodyMaterial->ShadingModel = Crystal::EShadingModel::ShadingModel_DefaultLit;

		auto tentacleMaterial = std::make_unique<Crystal::Material>();
		tentacleMaterial->ShadingModel = Crystal::EShadingModel::ShadingModel_DefaultLit;


		auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		sphereComponent->SetRadius(804 / 2.0f);
		
		sphereComponent->SetMass(40000.0f);
		
		
		auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
		skeletalMeshComponent->RotatePitch(90.0f);
		skeletalMeshComponent->AddMaterial(std::move(bodyMaterial));
		skeletalMeshComponent->AddMaterial(std::move(tentacleMaterial));
		skeletalMeshComponent->SetLocalPosition({ 0.0f, -400.0f , 0.0f });

		
		m_MainComponent = sphereComponent;

		skeletalMeshComponent->AttachTo(m_MainComponent);


		SetPosition({0.0f, 0.0f, 2000.0f});
	}


	STATIC_TYPE_IMPLE(Kraken)
};
