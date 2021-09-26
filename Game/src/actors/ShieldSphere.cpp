#include "ShieldSphere.h"

#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(ShieldSphere)

void ShieldSphere::Initialize()
{
	Crystal::Actor::Initialize();

	auto& resourceManager = Crystal::ResourceManager::Instance();
		
	auto material = Crystal::CreateShared<Crystal::Material>();
		
	material->EmissiveColor = { 80.0f / 255.0f * 3.0f, 226.0f / 255.0f * 3.0f, 0.0f };
	material->TexCoordMultiplier = { 7.0f, 7.0f };
	material->Opacity = 0.2f;
	material->OpacityTexture = resourceManager.GetTexture("assets/textures/bump.png");
		
	material->ShadingModel = Crystal::EShadingModel::SM_Unlit;
	material->BlendMode = Crystal::EBlendMode::BM_Translucent;
	material->bTwoSided = false;
		
	auto boundingSphereComponent = CreateComponent<Crystal::BoundingSphereComponent>("BoundingSphereComponent");
	boundingSphereComponent->SetRadius(2300.0f * 3.0f);
	boundingSphereComponent->IgnoreActorClassOf("Kraken");
	boundingSphereComponent->IgnoreActorClassOf("SpaceWhale");
	boundingSphereComponent->IgnoreActorClassOf("Drone");
	boundingSphereComponent->SetInverseMass(0.0f);
	m_MainComponent = boundingSphereComponent;
		

	auto staticMeshComponent = CreateComponent<Crystal::StaticMeshComponent>("StaticMeshComponent");

	staticMeshComponent->AddMaterial(std::move(material));
	staticMeshComponent->SetUnitScale(2300.0f * 3.0f);
	staticMeshComponent->SetRenderable(resourceManager.GetRenderable<Crystal::StaticMesh>("assets/models/1M_Sphere.fbx"));
	staticMeshComponent->AttachTo(m_MainComponent);
		
	m_MainComponent = staticMeshComponent;

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
}

void ShieldSphere::OnTakeDamage(float damage, Crystal::Weak<Actor> causer)
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
	if(m_CurrentHealth <= 0.0f)
	{
		Destroy();
		Crystal::Cast<Kraken>(GetInstigator())->SetPhase(2);
	}
}

void ShieldSphere::Update(float deltaTime)
{
	Actor::Update(deltaTime);

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
				m_HealthBarBgComponent->SetWorldPosition({ position2D.x, position2D.y, 1.0f });
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
}

void ShieldSphere::SetShowHealthBar(bool bShow)
{
	m_bShouldShowHealthBar = bShow;
	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Reset();
	}

	m_HealthBarBgComponent->SetHiddenInGame(!bShow);
	m_HealthBarFillComponent->SetHiddenInGame(!bShow);
}

void ShieldSphere::UpdateHealth() const
{
	float healthPercent = m_CurrentHealth / m_MaxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);
	m_HealthBarFillComponent->SetScaleX(m_HealthBarBgComponent->GetScale().x * healthPercent);
}
