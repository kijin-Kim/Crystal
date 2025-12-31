#include "LevelLandscapeActor.h"

#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

void LevelLandscapeActor::Initialize()
{
	Actor::Initialize();

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({ 600.0f / 2.0f, 580.0f / 2.0f, 2200.0f / 2.0f });
	boundingOrientedBoxComponent->SetMass(100000.0f);

	m_MainComponent = boundingOrientedBoxComponent;

	auto& resourceManager = Crystal::ResourceManager::Instance();


	auto cruiserMaterial = Crystal::CreateShared<Crystal::Material>();
	cruiserMaterial->EmissiveTexture = resourceManager.GetTexture("assets/textures/terrain1dif.jpeg");
	cruiserMaterial->ShadingModel = Crystal::EShadingModel::SM_Unlit;
	cruiserMaterial->BlendMode = Crystal::EBlendMode::BM_Opaque;

	auto terrainStaticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	terrainStaticMeshComponent->AddMaterial(cruiserMaterial);
	terrainStaticMeshComponent->AttachTo(m_MainComponent);
	terrainStaticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/terrain1.fbx"));


}


