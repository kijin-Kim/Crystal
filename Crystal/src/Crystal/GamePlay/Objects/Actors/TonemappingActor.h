#pragma once
#include "PlaneQuadActor.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {

class TonemappingActor : public PlaneQuadActor
{
public:
	TonemappingActor()
	{
		auto& resourceManager = ResourceManager::Instance();

		auto tonemappingMaterial = std::make_shared<Crystal::Material>();
		tonemappingMaterial->SetObjectOwner(resourceManager.GetShader("Tonemapping"),
			Crystal::Material::MaterialOwnerType::Owner_Shader);
		tonemappingMaterial->OnCreate();
		tonemappingMaterial->Set("FloatingPointTexture", resourceManager.GetTexture("FloatingPointBuffer"));


		m_StaticMeshComponent->AddMaterialOld(tonemappingMaterial);

	}

	~TonemappingActor() override = default;


	STATIC_TYPE_IMPLE(TonemappingActor)
};
}
