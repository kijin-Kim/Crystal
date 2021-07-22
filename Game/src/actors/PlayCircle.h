#pragma once
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
		material->Opacity = 0.2f;
		material->ShadingModel = Crystal::EShadingModel::SM_Unlit;
		material->BlendMode = Crystal::EBlendMode::BM_Translucent;
		material->bTwoSided = true;
		

		

		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");

		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->SetUnitScale(5.0f);
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Sphere.fbx"));
		
		

		m_MainComponent = staticMeshComponent;
	}

	STATIC_TYPE_IMPLE(PlayCircle)
};
