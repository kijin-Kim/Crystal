#pragma once
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/LightComponent.h"

class Sun : public Crystal::Actor
{
public:
	Sun()
	{
		auto& resourceManager = Crystal::ResourceManager::Instance();


		
		auto pbrMaterial = std::make_shared<Crystal::Material>();
		pbrMaterial->OnCreate();
		pbrMaterial->SetObjectOwner(resourceManager.GetShader("PBRShader_Static"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);
		pbrMaterial->Set("EmissiveColor", DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));


		auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
		staticMeshComponent->SetRenderable(resourceManager.GetRenderable("Sphere"));
		staticMeshComponent->AddMaterial(pbrMaterial);
		staticMeshComponent->SetScale(60.0f);

		m_MainComponent = staticMeshComponent;

		auto localLightComponent = CreateComponent<Crystal::LocalLightComponent>("LocalLightComponent");
		localLightComponent->SetLocalPosition(Crystal::Vector3::Zero);
		localLightComponent->SetLightColor({ 1.0f, 1.0f, 0.4f });
		localLightComponent->SetLightIntensity(2.0f);
		

	
		SetAttachment(localLightComponent, m_MainComponent);


	}
	~Sun() override = default;

	STATIC_TYPE_IMPLE(Sun)
};
