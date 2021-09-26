#include "AllyDroneLaser.h"

#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/ProjectileMovementComponent.h"
#include "Crystal/Resources/Material.h"

void AllyDroneLaser::Initialize()
{
	Actor::Initialize();

	auto material = std::make_unique<Crystal::Material>();

	material->ShadingModel = Crystal::EShadingModel::SM_Unlit;;
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->EmissiveColor = { 255.0f / 255.0f * 3.0f, 0.0f, 0.0f };

	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetRadius(15.0f);
	boundingSphereComponent->SetMass(1.0f);
	boundingSphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);
	boundingSphereComponent->IgnoreActorClassOf("AllyDrone");
	boundingSphereComponent->IgnoreActorClassOf("DroneLaser");
	boundingSphereComponent->IgnoreActorClassOf("MyPlayerPawn");
	boundingSphereComponent->IgnoreActorClassOf("PolluteSphere");
	boundingSphereComponent->IgnoreActorClassOf("Laser");

	boundingSphereComponent->BindOnBeginOverlapEvent([this](const Crystal::OverlapResult& overlapResult)
		{
			auto overlappedActor = overlapResult.OverlappedActor.lock();
			if (overlappedActor)
			{
				overlappedActor->OnTakeDamage(m_Damage, GetInstigator());
				Destroy();
			}
		});
	boundingSphereComponent->SetDamping(1.0f);


	m_MainComponent = boundingSphereComponent;

	auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	meshComponent->AddMaterial(std::move(material));
	meshComponent->AttachTo(m_MainComponent);
	meshComponent->SetUnitScale(5.0f);
	meshComponent->SetRenderable(Crystal::ResourceManager::Instance().GetRenderable<Crystal::StaticMesh>("assets/models/SM_Missile_A.fbx"));
	meshComponent->SetCastShadow(false);



	auto projectileMovementComponent = CreateComponent<Crystal::ProjectileMovementComponent>("ProjectileMovementComponent");
	projectileMovementComponent->SetTargetComponent(m_MainComponent);
	projectileMovementComponent->SetProjectileMaxAcceleration(200000.0f);
}

void AllyDroneLaser::Update(float deltaTime)
{
	Crystal::Actor::Update(deltaTime);

	m_LaserLifeTimer.Tick();

	if (m_LaserLifeTimer.GetElapsedTime() >= m_LaserLifeTime)
	{
		Destroy();
	}
}