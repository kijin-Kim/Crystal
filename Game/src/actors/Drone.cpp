#include "Drone.h"

#include "DroneLaser.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/World/Level.h"


void Drone::Initialize()
{
	Crystal::Pawn::Initialize();

	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetRadius(1.0f);
	boundingSphereComponent->SetMass(200.0f);
	boundingSphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);

	m_MainComponent = boundingSphereComponent;

	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto material = Crystal::CreateShared<Crystal::Material>();
	material->AlbedoColor = Crystal::Vector3::Red;

	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/SM_MK3InterceptorMainLite.fbx"));
	staticMeshComponent->AddMaterial(material);
	staticMeshComponent->SetUnitScale(2.0f);
	staticMeshComponent->AttachTo(m_MainComponent);

	m_FireSocketComponent = CreateComponent<Crystal::TransformComponent>("FireSocektComponent");
	m_FireSocketComponent->SetLocalPosition({ 0.0f, 0.0f, 20.0f });
	m_FireSocketComponent->AttachTo(m_MainComponent);
}

void Drone::OnFire()
{
	auto level = Crystal::Cast<Crystal::Level>(GetLevel());

	Crystal::Actor::ActorSpawnParams spawnParams = {};

	spawnParams.Position = m_FireSocketComponent->GetWorldPosition();
	spawnParams.Rotation = GetRotationQuat();
	level->SpawnActor<DroneLaser>(spawnParams);
}
