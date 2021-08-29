#include "MineCave.h"

#include "Crystal/Types.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

void MineCave::Initialize()
{
	auto material = Crystal::CreateShared<Crystal::Material>();
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;
	material->bTwoSided = true;



	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	staticMeshComponent->AddMaterial(material);


	m_MainComponent = staticMeshComponent;

	auto& resourceManager = Crystal::ResourceManager::Instance();

	material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Rock032_2K_Color.jpeg");
	staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/RealisticGoldMine3.fbx"));


}
