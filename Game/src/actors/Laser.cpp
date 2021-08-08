#include "Laser.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/ProjectileMovementComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

void Laser::Initialize()
{
	Crystal::PointLightActor::Initialize();
	
	auto material = std::make_unique<Crystal::Material>();

	material->ShadingModel = Crystal::EShadingModel::SM_Unlit;;
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;
	material->EmissiveColor = { 255.0f/ 255.0f * 3.0f, 127.0f / 255.0f * 3.0f, 0.0f };

	auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	sphereComponent->SetRadius(1.0f);
	sphereComponent->SetMass(1.0f);
	sphereComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);
	sphereComponent->IgnoreActorClassOf("MyPlayerPawn");
	sphereComponent->BindOnBeginOverlapEvent([this](const Crystal::OverlapResult&)
		{
			Destroy();
		});
	sphereComponent->SetDamping(1.0f);

	m_MainComponent = sphereComponent;

	auto meshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");
	meshComponent->AddMaterial(std::move(material));
	meshComponent->AttachTo(m_MainComponent);
	meshComponent->SetUnitScale(3.0f);
	meshComponent->SetRenderable(Crystal::ResourceManager::Instance().GetRenderable<Crystal::StaticMesh>("assets/models/SM_Missile_A.fbx"));

	


	auto projectileMovementComponent = CreateComponent<Crystal::ProjectileMovementComponent>("ProjectileMovementComponent");
	projectileMovementComponent->SetTargetComponent(m_MainComponent);
	projectileMovementComponent->SetProjectileMaxAcceleration(100000.0f);


	m_LightComponent->SetLightColor({ 255.0f / 255.0f * 3.0f, 127.0f / 255.0f * 3.0f, 0.0f });
	m_LightComponent->SetAttenuationRadius(500.0f);
	m_LightComponent->SetLightIntensity(3.0f);
	m_LightComponent->AttachTo(m_MainComponent);
}

void Laser::Update(float deltaTime)
{
	Crystal::PointLightActor::Update(deltaTime);

	m_LaserLifeTimer.Tick();

	if(m_LaserLifeTimer.GetElapsedTime() >= m_LaserLifeTime)
	{
		Destroy();
	}
}

