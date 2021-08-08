#include "PlayerShield.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/ResourceManager.h"

void PlayerShield::Initialize()
{
	Crystal::Actor::Initialize();

	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto material = Crystal::CreateShared<Crystal::Material>();
	material->EmissiveColor = Crystal::Vector3::Yellow;
	material->TexCoordMultiplier = { 3.0f, 3.0f };
	material->OpacityMultiplier = 0.3f;
	
	material->OpacityTexture = resourceManager.GetTexture("assets/textures/bump.png");

	material->ShadingModel = Crystal::EShadingModel::SM_Unlit;
	material->BlendMode = Crystal::EBlendMode::BM_Translucent;
	material->bTwoSided = false;

	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetRadius(53.0f);
	boundingSphereComponent->SetInverseMass(0.0f);
	boundingSphereComponent->IgnoreActorClassOf("MyPlayerPawn");
	boundingSphereComponent->IgnoreActorClassOf("Laser");
	m_MainComponent = boundingSphereComponent;;


	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");

	staticMeshComponent->AddMaterial(std::move(material));
	staticMeshComponent->SetUnitScale(53.0f);
	staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/1M_Sphere.fbx"));
	staticMeshComponent->AttachTo(m_MainComponent);

	m_MainComponent = staticMeshComponent;
}
