#pragma once
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Objects/Actors/LightActor.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

class PlayCircle : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	PlayCircle() = default;
	~PlayCircle() override = default;

	void Initialize() override
	{
		Crystal::Actor::Initialize();


		auto& resourceManager = Crystal::ResourceManager::Instance();
		
		auto material = Crystal::CreateShared<Crystal::Material>();
		material->EmissiveColor = Crystal::Vector3::Red;
		material->Opacity = 0.3f;
		//material->OpacityTexture = resourceManager.GetTexture("assets/textures/T_HoneycombMask.tga");
		
		material->ShadingModel = Crystal::EShadingModel::SM_Unlit;
		material->BlendMode = Crystal::EBlendMode::BM_Translucent;
		material->bTwoSided = true;
		
		auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
		boundingSphereComponent->SetRadius(50.0f);
		m_MainComponent = boundingSphereComponent;

		
		
		

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");

		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->SetUnitScale(50.0f);
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/1M_Sphere.fbx"));
		staticMeshComponent->AttachTo(m_MainComponent);
		
		

		m_MainComponent = staticMeshComponent;
	}

	STATIC_TYPE_IMPLE(PlayCircle)
};
