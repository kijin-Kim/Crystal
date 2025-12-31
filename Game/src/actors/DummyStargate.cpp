#include "DummyStargate.h"

#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"

void DummyStargate::Initialize()
{
	Crystal::Actor::Initialize();

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetUnitScale(100.0f * 10.0f);
	boundingOrientedBoxComponent->SetInverseMass(0.0f);
	boundingOrientedBoxComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);

	m_MainComponent = boundingOrientedBoxComponent;

	auto& resourceManager = Crystal::ResourceManager::Instance();


	auto gateMaterial = Crystal::CreateShared<Crystal::Material>();
	gateMaterial->AlbedoTexture = resourceManager.GetTexture("assets/textures/gate_albedo.png");
	gateMaterial->MetallicTexture = resourceManager.GetTexture("assets/textures/gate_metalness.png");
	gateMaterial->RoughnessTexture = resourceManager.GetTexture("assets/textures/gate_roughness.png");
	gateMaterial->ShadingModel = Crystal::EShadingModel::SM_Lit;


	auto gateMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	gateMeshComponent->AddMaterial(gateMaterial);
	gateMeshComponent->AttachTo(m_MainComponent);
	gateMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/stargate.fbx"));

	auto circleMaterial = Crystal::CreateShared<Crystal::Material>();
	circleMaterial->EmissiveColor = { 80.0f / 255.f * 3.0f, 188.0f / 255.0f * 3.0f, 223.0f / 255.0f * 3.0f };
	circleMaterial->Opacity = 0.3f;
	circleMaterial->TexCoordMultiplier = { 7.0f, 7.0f };
	circleMaterial->OpacityTexture = resourceManager.GetTexture("assets/textures/bump.png");
	circleMaterial->ShadingModel = Crystal::EShadingModel::SM_Unlit;
	circleMaterial->BlendMode = Crystal::EBlendMode::BM_Translucent;

	auto circleMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("CircleMeshComponent");
	circleMeshComponent->AddMaterial(circleMaterial);
	circleMeshComponent->AttachTo(m_MainComponent);
	circleMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/stargate_circle.fbx"));

}


