#include "Kraken.h"

#include "SpaceWhale.h"
#include "SpaceWhaleAIController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "MyHUD.h"
#include "MyPlayerPawn.h"
#include "PolluteSphere.h"
#include "ShieldSphere.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"


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

	m_ImpulseBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("ImpulseBoxComponent");
	m_ImpulseBoxComponent->SetExtents({ 500.0f, 500.0f, 1800.0f });
	m_ImpulseBoxComponent->SetCollisionType(Crystal::ECollisionType::CT_Overlap);


	m_SkeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
	m_SkeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/KRAKEN.fbx"));
	m_SkeletalMeshComponent->PlayAnimationWithEvent(resourceManager.GetAnimation("assets/models/KRAKEN_idle.fbx"), true, CS_ANIMATION_FN(Kraken::OnAttack),
	                                                3.0f);
	
	m_SkeletalMeshComponent->AddMaterial(std::move(bodyMaterial));
	m_SkeletalMeshComponent->AddMaterial(std::move(tentacleMaterial));
	m_SkeletalMeshComponent->RotatePitch(-90.0f);
	m_SkeletalMeshComponent->SetLocalPosition({0.0f, 400.0f, 0.0f});

	m_MainComponent = sphereComponent;

	m_SkeletalMeshComponent->AttachTo(m_MainComponent);
	m_ImpulseBoxComponent->AttachTo(m_MainComponent);
	


	auto barBgMat = Crystal::CreateShared<Crystal::Material>();
	barBgMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarBg.png");
	barBgMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarBgComponent = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
	m_HealthBarBgComponent->AddMaterial(barBgMat);
	m_HealthBarBgComponent->SetScaleX(0.072f);
	m_HealthBarBgComponent->SetScaleY(0.048f);


	auto hpBarFillMat = Crystal::CreateShared<Crystal::Material>();
	hpBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarFill.png");
	hpBarFillMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarWidth = hpBarFillMat->AlbedoTexture.lock()->GetWidth();
	m_HealthBarHeight = hpBarFillMat->AlbedoTexture.lock()->GetHeight();

	m_HealthBarFillComponent = CreateComponent<Crystal::TextureComponent>("HpFillTextureComponent");
	m_HealthBarFillComponent->AddMaterial(hpBarFillMat);
	m_HealthBarFillComponent->SetScaleX(0.072f * 0.5f);
	m_HealthBarFillComponent->SetScaleY(0.048f);

	SetShowHealthBar(false);
}

void Kraken::Begin()
{
	Crystal::Pawn::Begin();

	SetPhase(1);

	m_CurrentHealth = m_MaxHealth;

	UpdateHealth();
}

void Kraken::Update(float deltaTime)
{
	Crystal::Pawn::Update(deltaTime);


	if (m_CurrentPolluteGauge >= m_MaxPolluteGauge)
	{
		auto level = Crystal::Cast<Crystal::Level>(GetLevel());
		if (level)
		{
			auto playerPawn = Crystal::Cast<MyPlayerPawn>(level->GetPlayerPawn());

			if (playerPawn)
			{
				if(m_CurrentPhase == 3)
				{
					if(playerPawn->GetIsNotInPolluteSphere())
					{
						m_PolluteDamageTimer.Tick();
						if (m_PolluteDamageTimer.GetElapsedTime() >= m_PolluteDamageInterval)
						{
							m_PolluteDamageTimer.Reset();
							playerPawn->OnTakeDamage(2.0f, Crystal::Cast<Crystal::Actor>(weak_from_this()));
						}
					}

				}
				else
				{
					m_PolluteDamageTimer.Tick();
					if (m_PolluteDamageTimer.GetElapsedTime() >= m_PolluteDamageInterval)
					{
						m_PolluteDamageTimer.Reset();
						playerPawn->OnTakeDamage(2.0f, Crystal::Cast<Crystal::Actor>(weak_from_this()));
					}
				}

				
			}
		}
	}


	m_SpaceWhaleSpawnTimer.Tick();
	if (m_SpaceWhaleSpawnTimer.GetElapsedTime() >= m_SpaceWhaleSpawnInterval)
	{
		m_SpaceWhaleSpawnTimer.Reset();

		auto level = Crystal::Cast<Crystal::Level>(GetLevel());
		if (level)
		{
			for (int i = 0; i < 3; i++)
			{
				if (m_CurrentWhaleSpawnCount >= m_MaxWhaleSpawnCount)
				{
					break;
				}

				ActorSpawnParams spawnParams = {};
				spawnParams.Instigator = Crystal::Cast<Actor>(weak_from_this());

				auto spaceWhale = level->SpawnActor<SpaceWhale>(spawnParams).lock();
				spaceWhale->SetPosition(Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 6000.0f));
				spaceWhale->RotatePitch(Crystal::RandomFloatInRange(0.0f, 359.0f));
				spaceWhale->RotateYaw(Crystal::RandomFloatInRange(0.0f, 359.0f));
				spaceWhale->RotateRoll(Crystal::RandomFloatInRange(0.0f, 359.0f));

				auto spaceWhaleController = level->SpawnActor<SpaceWhaleAIController>({}).lock();
				spaceWhaleController->Possess(spaceWhale);
				++m_CurrentWhaleSpawnCount;
			}
		}
	}


	if(m_bShouldShowHealthBar)
	{
		auto level = Crystal::Cast<Crystal::Level>(GetLevel());
		if (level)
		{
			float healthPercent = m_CurrentHealth / m_MaxHealth;
			healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

			auto playerController = Crystal::Cast<Crystal::PlayerController>(level->GetPlayerController(0));
			if (playerController)
			{
				auto position2D = playerController->ProjectWorldToCameraSpace(GetPosition());
				position2D.y += 100.0f;
				m_HealthBarBgComponent->SetWorldPosition({ position2D.x, position2D.y, 2.0f });
				m_HealthBarFillComponent->SetWorldPosition({
					position2D.x - m_HealthBarWidth * m_HealthBarBgComponent->GetScale().x * (1.0f - healthPercent), position2D.y, 1.0f
					});
			}

		}

	}
	

	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Tick();
		if (m_HealthBarShowTimer.GetElapsedTime() >= m_HealthBarShowTime)
		{
			m_HealthBarShowTimer.Reset();
			SetShowHealthBar(false);
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
			hud->OnKrakenHealthUpdated(m_CurrentHealth, m_MaxHealth);
		}

		float healthPercent = m_CurrentHealth / m_MaxHealth;
		healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);
		m_HealthBarFillComponent->SetScaleX(m_HealthBarBgComponent->GetScale().x * healthPercent);
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
	if (damageCauser.lock()->StaticType() != "MyPlayerPawn")
	{
		return;
	}

	m_CurrentHealth -= damage;
	UpdateHealth();
	SetShowHealthBar(true);

	if (m_CurrentHealth / m_MaxHealth <= 0.5f)
	{
		if (GetPhase() != 3)
		{
			SetPhase(3);
		}
	}
}

void Kraken::SetPhase(uint32_t phase)
{
	m_CurrentPhase = phase;

	switch (m_CurrentPhase)
	{
	case 1:
		{
			auto level = Crystal::Cast<Crystal::Level>(GetLevel());
			if (level)
			{
				ActorSpawnParams spawnParams = {};
				spawnParams.Instigator = Crystal::Cast<Crystal::Actor>(weak_from_this());
				spawnParams.Position = GetPosition();

				level->SpawnActor<ShieldSphere>(spawnParams);
			}
		}
		break;
	case 3:
		{
			auto level = Crystal::Cast<Crystal::Level>(GetLevel());
			if (level)
			{
				ActorSpawnParams spawnParams = {};
				spawnParams.Instigator = Crystal::Cast<Crystal::Actor>(weak_from_this());
				spawnParams.Position = GetPosition();

				level->SpawnActor<PolluteSphere>(spawnParams);

				SetPolluteGauge(m_MaxPolluteGauge);
			}
		}
		break;
	}
}

void Kraken::SetPolluteGauge(float polluteGauge)
{
	m_CurrentPolluteGauge = polluteGauge;
	m_CurrentPolluteGauge = min(m_CurrentPolluteGauge, m_MaxPolluteGauge);


	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		auto hud = Crystal::Cast<MyHUD>(level->GetHUD());
		if (hud)
		{
			hud->OnPolluteGaugeUpdated(m_CurrentPolluteGauge, m_MaxPolluteGauge);
		}
	}


	if (m_CurrentPolluteGauge >= m_MaxPolluteGauge)
	{
		GreenTintVolumeActor->SetIsEnabled(true);
		GreenTintVolumeActor->SetHiddenInGame(false);
		VignetteVolumeActor->SetIsEnabled(true);
		VignetteVolumeActor->SetHiddenInGame(false);
	}
	else
	{
		GreenTintVolumeActor->SetIsEnabled(false);
		GreenTintVolumeActor->SetHiddenInGame(true);
		VignetteVolumeActor->SetIsEnabled(false);
		VignetteVolumeActor->SetHiddenInGame(true);
	}
}

void Kraken::SetShowHealthBar(bool bShow)
{
	m_bShouldShowHealthBar = bShow;
	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Reset();
	}

	m_HealthBarBgComponent->SetHiddenInGame(!bShow);
	m_HealthBarFillComponent->SetHiddenInGame(!bShow);
}
