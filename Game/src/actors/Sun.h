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
		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto pbrMaterial = std::make_shared<Crystal::Material>();
		pbrMaterial->OnCreate();
		pbrMaterial->SetObjectOwner(resourceManager.GetShader("PBRShader_Static"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);
		pbrMaterial->Set("EmissiveColor", DirectX::XMFLOAT3(1.0f * 3.0f, 1.0f * 3.0f, 0.4f * 3.0f));

		//auto material = std::make_unique<Crystal::LightingStaticPipeline::Material>();
		auto material = std::make_unique<Crystal::NewMaterial>();


		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
//		staticMeshComponent->SetRenderable(resourceManager.GetRenderable("Sphere"));
		staticMeshComponent->AddMaterialOld(pbrMaterial);
		staticMeshComponent->AddMaterial(std::move(material));
		staticMeshComponent->SetScale(600.0f);

		m_MainComponent = staticMeshComponent;

		
		m_LightComponent->SetLocalPosition(Crystal::Vector3::Zero);
		m_LightComponent->SetLightColor(Crystal::Vector3::White);
		m_LightComponent->SetLightIntensity(3.0f);
		

	
		SetAttachment(m_LightComponent, m_MainComponent);
	}
	~Sun() override = default;

	STATIC_TYPE_IMPLE(Sun)
};
