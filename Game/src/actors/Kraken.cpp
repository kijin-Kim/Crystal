#include "Kraken.h"

#include "SpaceWhale.h"
#include "SpaceWhaleAIController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "MyHUD.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

BOOST_CLASS_EXPORT(Kraken)

void Kraken::Initialize()
{
	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto bodyMaterial = std::make_unique<Crystal::Material>();
	bodyMaterial->AlbedoTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_BODY_BaseColor.tga");
	bodyMaterial->RoughnessTexture = resourceManager.GetTexture("assets/textures/Kraken/T_M_KRAKEN_Mat_KRAKEN_MAIN_BODY_Roughness.tga");
	bodyMaterial->NormalTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_BODY_NRM.tga");
	bodyMaterial->ShadingModel = Crystal::EShadingModel::SM_Lit;
	bodyMaterial->BlendMode = Crystal::EBlendMode::BM_Opaque;

	auto tentacleMaterial = std::make_unique<Crystal::Material>();
	tentacleMaterial->AlbedoTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_BaseColor.tga");
	tentacleMaterial->RoughnessTexture = resourceManager.GetTexture("assets/textures/Kraken/T_M_KRAKEN_Mat_TENTACLES_LEGS_CLAWS_Roughness.tga");
	tentacleMaterial->NormalTexture = resourceManager.GetTexture("assets/textures/Kraken/Tex_KRAKEN_LEG_TENTACLE_CLAW_NRM.tga");
	tentacleMaterial->ShadingModel = Crystal::EShadingModel::SM_Lit;
	tentacleMaterial->BlendMode = Crystal::EBlendMode::BM_Opaque;


	auto sphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	sphereComponent->SetRadius(804 / 2.0f);

	sphereComponent->SetInverseMass(0.0f);


	m_SkeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
	m_SkeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/KRAKEN.fbx"));
	m_SkeletalMeshComponent->PlayAnimationWithEvent(resourceManager.GetAnimation("assets/models/KRAKEN_idle.fbx"), true, CS_ANIMATION_FN(Kraken::OnAttack),
	                                                3.0f);
	m_SkeletalMeshComponent->RotatePitch(90.0f);
	m_SkeletalMeshComponent->AddMaterial(std::move(bodyMaterial));
	m_SkeletalMeshComponent->AddMaterial(std::move(tentacleMaterial));
	m_SkeletalMeshComponent->SetLocalPosition({0.0f, -400.0f, 0.0f});


	m_MainComponent = sphereComponent;

	m_SkeletalMeshComponent->AttachTo(m_MainComponent);
}

void Kraken::Begin()
{
	Crystal::Pawn::Begin();

	m_Health = m_MaxHealth;

	UpdateHealth();
}

void Kraken::Update(float deltaTime)
{
	Crystal::Pawn::Update(deltaTime);

	m_SpaceWhaleSpawnTimer.Tick();
	if (m_SpaceWhaleSpawnTimer.GetElapsedTime() >= m_SpaceWhaleSpawnInterval)
	{
		m_SpaceWhaleSpawnTimer.Reset();

		auto level = Crystal::Cast<Crystal::Level>(GetLevel());
		if (level)
		{
			for (int i = 0; i < 3; i++)
			{
				auto spaceWhale = level->SpawnActor<SpaceWhale>({}).lock();
				spaceWhale->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 6000.0f));
				spaceWhale->RotatePitch(Crystal::RandomFloatInRange(0.0f, 359.0f));
				spaceWhale->RotateYaw(Crystal::RandomFloatInRange(0.0f, 359.0f));
				spaceWhale->RotateRoll(Crystal::RandomFloatInRange(0.0f, 359.0f));

				auto spaceWhaleController = level->SpawnActor<SpaceWhaleAIController>({}).lock();
				spaceWhaleController->Possess(spaceWhale);
			}
		}
	}
}

void Kraken::UpdateHealth()
{
	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
		if (hud)
		{
			hud->OnKrakenHealthUpdated(m_Health, m_MaxHealth);
		}
	}
}

void Kraken::OnAttack()
{
	auto& resourceManager = Crystal::ResourceManager::Instance();
	m_SkeletalMeshComponent->PlayAnimationWithEndEvent(resourceManager.GetAnimation("assets/models/KRAKEN_smashAttack.fbx"), false,
	                                                   CS_ANIMATION_FN(Kraken::OnIdle));
}

void Kraken::OnIdle()
{
	auto& resourceManager = Crystal::ResourceManager::Instance();
	m_SkeletalMeshComponent->PlayAnimationWithEvent(resourceManager.GetAnimation("assets/models/KRAKEN_idle.fbx"), true, CS_ANIMATION_FN(Kraken::OnAttack),
	                                                3.0f);
}

void Kraken::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	m_Health -= damage;
	UpdateHealth();
}
