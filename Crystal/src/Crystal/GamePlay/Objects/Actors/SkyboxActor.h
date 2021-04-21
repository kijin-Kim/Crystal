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

			//auto skyboxMaterial = std::make_shared<Crystal::Material>();
			//skyboxMaterial->SetObjectOwner(resourceManager.GetShader("Skybox"),
			//	Crystal::Material::MaterialOwnerType::Owner_Shader);
			//skyboxMaterial->OnCreate();
			//skyboxMaterial->Set("CubemapColorTexture", resourceManager.GetTexture("Cube_Skybox_Space"));
			//
			//
			//
			////==== ComputeShader Materials ========================
			//auto panoToCubeMaterial = std::make_shared<Crystal::Material>();
			//panoToCubeMaterial->SetObjectOwner(resourceManager.GetShader("PanoToCubemap"),
			//	Crystal::Material::MaterialOwnerType::Owner_Shader);
			//panoToCubeMaterial->OnCreate();
			//panoToCubeMaterial->Set("EquiTexture", resourceManager.GetTexture("Pano_Skybox_Space"));
			//panoToCubeMaterial->Set("OutputTexture", resourceManager.GetTexture("Cube_Skybox_Space"));

			//auto diffIrradSamplingMaterial = std::make_shared<Crystal::Material>();
			//diffIrradSamplingMaterial->SetObjectOwner(resourceManager.GetShader("DiffuseIrradianceSampling"),
			//	Crystal::Material::MaterialOwnerType::Owner_Shader);
			//diffIrradSamplingMaterial->OnCreate();
			//diffIrradSamplingMaterial->Set("CubemapTexture", resourceManager.GetTexture("Cube_Skybox_Space"));
			//diffIrradSamplingMaterial->Set("OutputTexture", resourceManager.GetTexture("Cube_Skybox_Space_Irradiance"));
			//=====================================================

			

			auto staticMeshComponent = CreateComponent<StaticMeshComponent>("StaticMeshComponent");
			staticMeshComponent->SetRenderable(resourceManager.GetRenderable("2DPlaneQuadMesh"));
			/*staticMeshComponent->AddMaterialOld(skyboxMaterial);
			staticMeshComponent->AddMaterialOld(panoToCubeMaterial);
			staticMeshComponent->AddMaterialOld(diffIrradSamplingMaterial);*/

			m_MainComponent = staticMeshComponent;

		}

		~SkyboxActor() override = default;


		STATIC_TYPE_IMPLE(SkyboxActor)
	};
}
