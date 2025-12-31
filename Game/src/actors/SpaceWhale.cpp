#include "SpaceWhale.h"

#include "Kraken.h"
#include "MyHUD.h"
#include "MyPlayerPawn.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"

void SpaceWhale::Initialize()
{
	Crystal::Pawn::Initialize();

	auto& resourceManager = Crystal::ResourceManager::Instance();

	auto material = Crystal::CreateShared<Crystal::Material>();
	material->AlbedoTexture = resourceManager.GetTexture("assets/textures/Whale Diffuse.jpg");
	material->RoughnessTexture = resourceManager.GetTexture("assets/textures/Whale Roughness.jpg");
	material->NormalTexture = resourceManager.GetTexture("assets/textures/Whale Normals.jpg");
	material->EmissiveTexture = resourceManager.GetTexture("assets/textures/Whale Emissive.jpg");
	material->ShadingModel = Crystal::EShadingModel::SM_Lit;
	material->BlendMode = Crystal::EBlendMode::BM_Opaque;

	auto boundingOrientedBoxComponent = CreateComponent<Crystal::BoundingOrientedBoxComponent>("BoundingOrientedBoxComponent");
	boundingOrientedBoxComponent->SetExtents({4.9f / 2.0f * 100.0f * 3.0f, 2.8f / 2.0f * 100.0f * 3.0f, 11.0f / 2.0f * 100.0f * 3.0f});
	boundingOrientedBoxComponent->SetMass(80000.0f);
	boundingOrientedBoxComponent->IgnoreActorClassOf("SpaceWhale");
	boundingOrientedBoxComponent->IgnoreActorClassOf("Kraken");
	boundingOrientedBoxComponent->BindOnHitEvent([this](const Crystal::HitResult& hitResult)
	{
		auto hitActor = hitResult.HitActor.lock();
		if(!hitActor)
		{
			return;
		}

		auto staticType = hitActor->StaticType();
		if (staticType == "BoundingOrientedBoxActor")
		{
			auto spaceWhaleController = Crystal::Cast<SpaceWhaleAIController>(GetController());
			if(spaceWhaleController)
			{
				auto blackboardComponent = spaceWhaleController->GetBlackboardComponent();
				if(blackboardComponent)
				{
					blackboardComponent->SetValueAsFloat3("RandomPositionInSphere", Crystal::Vector3::RandomPositionInSphere(Crystal::Vector3::Zero, 300.0f));
				}
			}
		}

		if (staticType == "MyPlayerPawn")
		{
			auto playerPawn = Crystal::Cast<MyPlayerPawn>(hitResult.HitActor);
			if (playerPawn)
			{
				playerPawn->OnTakeDamage(m_Power, Crystal::Cast<Crystal::Actor>(weak_from_this()));
			}

			auto spaceWhaleController = Crystal::Cast<SpaceWhaleAIController>(GetController());
			if (spaceWhaleController)
			{
				auto blackboardComponent = spaceWhaleController->GetBlackboardComponent();
				if(blackboardComponent)
				{
					blackboardComponent->SetValueAsBool("bDamagedPlayer", true);
				}
			}
		}
	});


	auto skeletalMeshComponent = CreateComponent<Crystal::SkeletalMeshComponent>("MeshComponent");
	skeletalMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::SkeletalMesh>("assets/models/Biomechanical Whale Animated.fbx"));
	skeletalMeshComponent->PlayAnimation(resourceManager.GetAnimation("assets/models/Biomechanical Whale Animated.fbx"), true);
	skeletalMeshComponent->AddMaterial(material);
	skeletalMeshComponent->SetUnitScale(100.0f * 3.0f);
	skeletalMeshComponent->RotateYaw(180.0f);

	m_MainComponent = boundingOrientedBoxComponent;

	skeletalMeshComponent->AttachTo(m_MainComponent);

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

	UpdateHealth();


	m_DamagePerceptionTimer.SetElapsedTime(m_DamagePerceptionInterval);
}

void SpaceWhale::End()
{
	Crystal::Pawn::End();

	auto kraken = Crystal::Cast<Kraken>(GetInstigator());
	if (kraken)
	{
		if (kraken->GetPhase() != 3)
		{
			kraken->SetPolluteGauge(kraken->GetCurrentPolluteGauge() - 10.0f);
		}
	}
}


void SpaceWhale::Update(float deltaTime)
{
	Crystal::Pawn::Update(deltaTime);

	m_PolluteGaugeIncrementTimer.Tick();
	if (m_PolluteGaugeIncrementTimer.GetElapsedTime() >= m_PolluteGaugeIncrementInterval)
	{
		m_PolluteGaugeIncrementTimer.Reset();

		auto kraken = Crystal::Cast<Kraken>(GetInstigator());
		if (kraken)
		{
			kraken->SetPolluteGauge(kraken->GetCurrentPolluteGauge() + 1.5f);
		}
	}

	if (m_bShouldShowHealthBar)
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
				m_HealthBarBgComponent->SetWorldPosition({position2D.x, position2D.y, 1.0f});
				m_HealthBarFillComponent->SetWorldPosition({
					position2D.x - m_HealthBarWidth * m_HealthBarBgComponent->GetScale().x * (1.0f - healthPercent), position2D.y, 2.0f
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

	m_DamagePerceptionTimer.Tick();
}

void SpaceWhale::OnTakeDamage(float damage, Crystal::Weak<Actor> causer)
{
	auto damageCauser = causer.lock();
	if (!damageCauser)
	{
		return;
	}

	if (damageCauser->StaticType() != "MyPlayerPawn")
	{
		return;
	}

	m_CurrentHealth -= damage;
	UpdateHealth();
	SetShowHealthBar(true);
	if (m_CurrentHealth <= 0.0f)
	{
		Destroy();
	}

	if (m_DamagePerceptionTimer.GetElapsedTime() >= m_DamagePerceptionInterval)
	{
		m_DamagePerceptionTimer.Reset();

		auto spaceWhaleController = Crystal::Cast<SpaceWhaleAIController>(GetController());
		if (spaceWhaleController)
		{
			spaceWhaleController->GetBlackboardComponent()->SetValueAsFloat3("PlayerLocation", damageCauser->GetPosition());
		}
	}
}

void SpaceWhale::SetShowHealthBar(bool bShow)
{
	m_bShouldShowHealthBar = bShow;
	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Reset();
	}

	m_HealthBarBgComponent->SetHiddenInGame(!bShow);
	m_HealthBarFillComponent->SetHiddenInGame(!bShow);
}

void SpaceWhale::UpdateHealth() const
{
	float healthPercent = m_CurrentHealth / m_MaxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);
	m_HealthBarFillComponent->SetScaleX(m_HealthBarBgComponent->GetScale().x * healthPercent);
}


