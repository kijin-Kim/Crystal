#include "DroneLaser.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/ProjectileMovementComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

void DroneLaser::Initialize()
{
	Crystal::PointLightActor::Initialize();

	auto material = std::make_unique<Crystal::Material>();

	material->ShadingModel = Crystal::EShadingModel::SM_Unlit;;
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->EmissiveColor = { 191.0f / 255.0f * 3.0f, 1.0f * 3.0f, 0.0f };

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({ 2.0f, 2.0f, 18.0f });
	boundingOrientedBoxComponent->SetMass(1.0f);
	boundingOrientedBoxComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);
	boundingOrientedBoxComponent->IgnoreActorClassOf("Drone");
	boundingOrientedBoxComponent->IgnoreActorClassOf("PolluteSphere");
	boundingOrientedBoxComponent->BindOnBeginOverlapEvent([this](const Crystal::OverlapResult& overlapResult)
		{
			auto overlappedActor = overlapResult.OverlappedActor.lock();
			if(overlappedActor)
			{
				overlappedActor->OnTakeDamage(m_Damage, GetInstigator());
				Destroy();
			}
		});
	boundingOrientedBoxComponent->SetDamping(1.0f);


	m_MainComponent = boundingOrientedBoxComponent;

	auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	meshComponent->AddMaterial(std::move(material));
	meshComponent->AttachTo(m_MainComponent);
	meshComponent->SetUnitScale(3.0f);
	meshComponent->SetRenderable(Crystal::ResourceManager::Instance().GetRenderable<Crystal::StaticMesh>("assets/models/SM_Missile_A.fbx"));
	meshComponent->SetCastShadow(false);



	auto projectileMovementComponent = CreateComponent<Crystal::ProjectileMovementComponent>("ProjectileMovementComponent");
	projectileMovementComponent->SetTargetComponent(m_MainComponent);
	projectileMovementComponent->SetProjectileMaxAcceleration(100000.0f);


	m_LightComponent->SetLightColor({ 191.0f / 255.0f * 3.0f, 1.0f * 3.0f, 0.0f });
	m_LightComponent->SetAttenuationRadius(500.0f);
	m_LightComponent->SetLightIntensity(3.0f);
	m_LightComponent->AttachTo(m_MainComponent);
}

void DroneLaser::Update(float deltaTime)
{
	Crystal::PointLightActor::Update(deltaTime);

	m_LaserLifeTimer.Tick();

	if (m_LaserLifeTimer.GetElapsedTime() >= m_LaserLifeTime)
	{
		Destroy();
	}
}


