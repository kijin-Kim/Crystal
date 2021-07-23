#pragma once
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Objects/Actors/LightActor.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

class Sun : public Crystal::DirectionalLightActor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::DirectionalLightActor>(*this);
	}

public:
	Sun() = default;
	~Sun() override = default;

	void Initialize() override
	{
		Crystal::DirectionalLightActor::Initialize();


		auto material = std::make_unique<Crystal::Material>();
		material->ShadingModel = Crystal::EShadingModel::SM_Unlit;
		material->BlendMode = Crystal::EBlendMode::BM_Opaque;



		auto& resourceManager = Crystal::ResourceManager::Instance();
		
		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");

		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->SetUnitScale(30.0f);
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/Sphere.fbx"));
		
		

		m_MainComponent = staticMeshComponent;


		m_LightComponent->SetLocalPosition(Crystal::Vector3::Zero);
		m_LightComponent->AttachTo(m_MainComponent);
	}

	STATIC_TYPE_IMPLE(Sun)
};
