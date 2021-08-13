#include "PlayerShield.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/ResourceManager.h"

void PlayerShield::Initialize()
{
	Crystal::Actor::Initialize();

	auto& resourceManager = Crystal::ResourceManager::Instance();


	auto barBgMat = Crystal::CreateShared<Crystal::Material>();
	barBgMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/hpBarBg.png");
	barBgMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarBgComponent = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
	m_HealthBarBgComponent->AddMaterial(barBgMat);
	m_HealthBarBgComponent->SetScaleX(0.072f);
	m_HealthBarBgComponent->SetScaleY(0.048f);


	auto hpBarFillMat = Crystal::CreateShared<Crystal::Material>();
	hpBarFillMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/playerHpBarFill.png");
	hpBarFillMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarWidth = hpBarFillMat->AlbedoTexture.lock()->GetWidth();
	m_HealthBarHeight = hpBarFillMat->AlbedoTexture.lock()->GetHeight();

	m_HealthBarFillComponent = CreateComponent<Crystal::TextureComponent>("HpFillTextureComponent");
	m_HealthBarFillComponent->AddMaterial(hpBarFillMat);
	m_HealthBarFillComponent->SetScaleX(0.072f * 0.5f);
	m_HealthBarFillComponent->SetScaleY(0.048f);

	SetShowHealthBar(true);
	UpdateHealth();
}

void PlayerShield::Update(float deltaTime)
{
	Actor::Update(deltaTime);

	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if (level)
	{
		float healthPercent = m_CurrentHealth / m_MaxHealth;
		healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);


		auto playerController = Crystal::Cast<Crystal::PlayerController>(level->GetPlayerController(0));
		if(playerController)
		{
			auto position2D = playerController->ProjectWorldToCameraSpace(GetPosition());
			position2D.y += 100.0f;
			m_HealthBarBgComponent->SetWorldPosition({ position2D.x, position2D.y, 2.0f });
			m_HealthBarFillComponent->SetWorldPosition({
				position2D.x - m_HealthBarWidth * m_HealthBarBgComponent->GetScale().x * (1.0f - healthPercent), position2D.y, 1.0f
				});
		}
		
	}

	OnTakeDamage(deltaTime * 1.0f, {});
}

void PlayerShield::OnTakeDamage(float damage, Crystal::Weak<Actor> damageCauser)
{
	m_CurrentHealth -= damage;
	UpdateHealth();
	if(m_CurrentHealth <= 0.0f)
	{
		Destroy();
	}
}

void PlayerShield::SetShowHealthBar(bool bShow)
{
	m_bShouldShowHealthBar = bShow;
	if (m_bShouldShowHealthBar)
	{
		m_HealthBarShowTimer.Reset();
	}

	m_HealthBarBgComponent->SetHiddenInGame(!bShow);
	m_HealthBarFillComponent->SetHiddenInGame(!bShow);
}

void PlayerShield::UpdateHealth() const
{
	float healthPercent = m_CurrentHealth / m_MaxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);
	m_HealthBarFillComponent->SetScaleX(m_HealthBarBgComponent->GetScale().x * healthPercent);
}
