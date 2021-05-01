#pragma once
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Objects/Actors/LightActor.h"

class Sun : public Crystal::LocalLightActor
{
public:
	Sun()
	{
	}
	~Sun() override = default;

	void Initialize() override
	{
		Crystal::LocalLightActor::Initialize();
		

		auto material = std::make_unique<Crystal::NewMaterial>();
		material->ShadingModel = Crystal::EShadingModel::ShadingModel_DefaultLit;


		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		//		staticMeshComponent->SetRenderable(resourceManager.GetRenderable("Sphere"));

		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->SetScale(600.0f);

		m_MainComponent = staticMeshComponent;


		m_LightComponent->SetLocalPosition(Crystal::Vector3::Zero);
		m_LightComponent->SetLightColor(Crystal::Vector3::White);
		m_LightComponent->SetLightIntensity(3.0f);



		SetAttachment(m_LightComponent, m_MainComponent);
	}

	STATIC_TYPE_IMPLE(Sun)
};
