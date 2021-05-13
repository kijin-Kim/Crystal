#pragma once
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Objects/Actors/LightActor.h"

class Sun : public Crystal::LocalLightActor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::LocalLightActor>(*this);
	}

public:
	Sun() = default;
	~Sun() override = default;

	void Initialize() override
	{
		Crystal::LocalLightActor::Initialize();


		auto material = std::make_unique<Crystal::Material>();
		material->ShadingModel = Crystal::EShadingModel::ShadingModel_DefaultLit;


		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");

		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->SetScale(600.0f);

		m_MainComponent = staticMeshComponent;


		m_LightComponent->SetLocalPosition(Crystal::Vector3::Zero);
		m_LightComponent->SetLightColor(Crystal::Vector3::White);
		m_LightComponent->SetLightIntensity(3.0f);
		m_LightComponent->AttachTo(m_MainComponent);
	}

	STATIC_TYPE_IMPLE(Sun)
};
