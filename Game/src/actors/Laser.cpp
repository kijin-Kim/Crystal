#include "Laser.h"

#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/ProjectileMovementComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

void Laser::Initialize()
{
	auto material = std::make_unique<Crystal::Material>();

	material->ShadingModel = Crystal::EShadingModel::SM_Unlit;;
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->EmissiveColor = {120 / 255.0f * 10.0f, 250 / 255.0f * 10.0f, 0.7f * 10.0f};

	auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	sphereComponent->SetRadius(1.0f);
	sphereComponent->SetMass(1.0f);
	sphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);
	sphereComponent->IgnoreActorClassOf("MyPlayerPawn");
	sphereComponent->BindOnBeginOverlapEvent([this](const Crystal::OverlapResult&)
		{
			Destroy();
		});

	m_MainComponent = sphereComponent;

	auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	meshComponent->AddMaterial(std::move(material));
	meshComponent->AttachTo(m_MainComponent);
	meshComponent->SetUnitScale(3.0f);
	meshComponent->SetRenderable(Crystal::ResourceManager::Instance().GetRenderable<Crystal::StaticMesh>("assets/models/SM_Missile_A.fbx"));

	


	auto projectileMovementComponent = CreateComponent<Crystal::ProjectileMovementComponent>("ProjectileMovementComponent");
	projectileMovementComponent->SetTargetComponent(m_MainComponent);
	projectileMovementComponent->SetProjectileMaxAcceleration(300000.0f);

}

