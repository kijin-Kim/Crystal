#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {
	class SkyboxActor : public Actor
	{
	public:
		SkyboxActor()
		{
			auto& resourceManager = ResourceManager::Instance();


			auto skyboxMaterial = std::make_shared<Crystal::Material>();
			skyboxMaterial->SetObjectOwner(resourceManager.GetShader("Skybox"),
				Crystal::Material::MaterialOwnerType::Owner_Shader);
			skyboxMaterial->Set("CubemapTexture", resourceManager.GetTexture("Cube_Skybox_Space"));


			auto staticMeshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable("PlaneQuadMesh"));
			staticMeshComponent->AddMaterial(skyboxMaterial);

			m_MainComponent = staticMeshComponent;

		}

		~SkyboxActor() override = default;


		STATIC_TYPE_IMPLE(SkyboxActor)
	};
}
