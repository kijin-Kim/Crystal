#include "MyHUD.h"

#include "MyPlayerPawn.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/ResourceManager.h"

void MyHUD::Initialize()
{
	auto crossHairMat = Crystal::CreateShared<Crystal::Material>();
	crossHairMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/RedCrosshair.tga");
	crossHairMat->bUseAlbedoTextureAlpha = true;

	auto crossHairTexture = CreateComponent<Crystal::TextureComponent>("CrosshairTextureComponent");
	crossHairTexture->AddMaterial(crossHairMat);
	crossHairTexture->SetUnitScale(0.8f);
	crossHairTexture->SetHiddenInGame(false);


	auto hitMarkerMat = Crystal::CreateShared<Crystal::Material>();
	hitMarkerMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hitmarker.png");
	hitMarkerMat->bUseAlbedoTextureAlpha = true;
	hitMarkerMat->Opacity = 0.0f;

	m_HitmarkerTextureComponent = CreateComponent<Crystal::TextureComponent>("HitmarkerTextureComponent");
	m_HitmarkerTextureComponent->AddMaterial(hitMarkerMat);
	m_HitmarkerTextureComponent->SetUnitScale(0.035f);
	m_HitmarkerTextureComponent->SetHiddenInGame(false);
	
	


	auto barBgMat = Crystal::CreateShared<Crystal::Material>();
	barBgMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarBg.png");
	barBgMat->bUseAlbedoTextureAlpha = true;

	auto hpBarBg = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
	hpBarBg->AddMaterial(barBgMat);
	hpBarBg->SetUnitScale(0.35f);
	hpBarBg->SetScaleY(0.13f);
	hpBarBg->SetWorldPosition({-1920.0f / 2.0f + 280.0f, -430.0f, 1.0f});
	hpBarBg->SetHiddenInGame(false);


	auto hpBarFillMat = Crystal::CreateShared<Crystal::Material>();
	hpBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/playerHpBarFill.png");
	hpBarFillMat->bUseAlbedoTextureAlpha = true;

	m_HealthBarWidth = hpBarFillMat->AlbedoTexture.lock()->GetWidth();
	m_HealthBarHeight = hpBarFillMat->AlbedoTexture.lock()->GetHeight();

	m_HealthFillComponent = CreateComponent<Crystal::TextureComponent>("HpFillTextureComponent");
	m_HealthFillComponent->SetWorldPosition({-1920.0f / 2.0f + 280.0f - m_HealthBarWidth * 0.35f * 0.5f, -430.0f, 0.0f});
	m_HealthFillComponent->AddMaterial(hpBarFillMat);
	m_HealthFillComponent->SetUnitScale(0.35f);
	m_HealthFillComponent->SetScaleX(0.35f * 0.5f);
	m_HealthFillComponent->SetScaleY(0.13f);
	m_HealthFillComponent->SetHiddenInGame(false);



	auto enemyHpBarBg = CreateComponent<Crystal::TextureComponent>("EnemyHpBackgroundTextureComponent");
	enemyHpBarBg->AddMaterial(barBgMat);
	enemyHpBarBg->SetUnitScale(0.7f);
	enemyHpBarBg->SetScaleY(0.20f);
	enemyHpBarBg->SetWorldPosition({ 0.0f, 430.0f, 1.0f });
	enemyHpBarBg->SetHiddenInGame(false);

	auto enemyHpBarFillMat = Crystal::CreateShared<Crystal::Material>();
	enemyHpBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/HpBarFill.png");
	enemyHpBarFillMat->bUseAlbedoTextureAlpha = true;

	m_EnemyHpBarFillComponent = CreateComponent<Crystal::TextureComponent>("EnemyHpFillTextureComponent");
	m_EnemyHpBarFillComponent->SetWorldPosition({ 0.0f - m_HealthBarWidth * 0.7f * 0.5f, 430.0f, 0.0f });
	m_EnemyHpBarFillComponent->AddMaterial(enemyHpBarFillMat);
	m_EnemyHpBarFillComponent->SetUnitScale(0.7f);
	m_EnemyHpBarFillComponent->SetScaleX(0.7f * 0.5f);
	m_EnemyHpBarFillComponent->SetScaleY(0.20f);
	m_EnemyHpBarFillComponent->SetHiddenInGame(false);

		
	auto pollutionHpBarBg = CreateComponent<Crystal::TextureComponent>("PollutionBackgroundTextureComponent");
	pollutionHpBarBg->AddMaterial(barBgMat);
	pollutionHpBarBg->SetUnitScale(0.7f);
	pollutionHpBarBg->SetScaleY(0.10f);
	pollutionHpBarBg->SetWorldPosition({ 0.0f, 430.0f - m_HealthBarHeight / 2.0f, 1.0f });
	pollutionHpBarBg->SetHiddenInGame(false);

	auto pollutionBarFillMat = Crystal::CreateShared<Crystal::Material>();
	pollutionBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/PollutionBarFill.png");
	pollutionBarFillMat->bUseAlbedoTextureAlpha = true;

	m_PollutionBarFillComponent = CreateComponent<Crystal::TextureComponent>("PollutionFillTextureComponent");
	m_PollutionBarFillComponent->SetWorldPosition({ 0.0f - m_HealthBarWidth * 0.7f * 0.5f, 430.0f - m_HealthBarHeight / 2.0f, 0.0f });
	m_PollutionBarFillComponent->AddMaterial(pollutionBarFillMat);
	m_PollutionBarFillComponent->SetUnitScale(0.7f);
	m_PollutionBarFillComponent->SetScaleX(0.7f * 0.0f);
	m_PollutionBarFillComponent->SetScaleY(0.10f);
	m_PollutionBarFillComponent->SetHiddenInGame(false);
		




	auto itemBg = Crystal::CreateShared<Crystal::Material>();
	itemBg->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/ItemBg.png");
	itemBg->bUseAlbedoTextureAlpha = true;


	auto itemBg1 = CreateComponent<Crystal::TextureComponent>("itemBg1Component");
	itemBg1->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 1) + (15.0f * 1), -430.0f, 0.0f});
	itemBg1->AddMaterial(itemBg);
	itemBg1->SetUnitScale(0.08f);
	itemBg1->SetHiddenInGame(false);

	auto itemBg2 = CreateComponent<Crystal::TextureComponent>("itemBg2Component");
	itemBg2->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 2) + (15.0f * 2), -430.0f, 0.0f});
	itemBg2->AddMaterial(itemBg);
	itemBg2->SetUnitScale(0.08f);
	itemBg2->SetHiddenInGame(false);

	auto itemBg3 = CreateComponent<Crystal::TextureComponent>("itemBg3Component");
	itemBg3->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 3) + (15.0f * 3), -430.0f, 0.0f});
	itemBg3->AddMaterial(itemBg);
	itemBg3->SetUnitScale(0.08f);
	itemBg3->SetHiddenInGame(false);


	auto powerPotionMat = Crystal::CreateShared<Crystal::Material>();
	powerPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/electric_potion.tga");
	powerPotionMat->bUseAlbedoTextureAlpha = true;

	m_PowerPotionIcon = CreateComponent<Crystal::TextureComponent>("PowerPotionIconComponent");
	m_PowerPotionIcon->SetWorldPosition({ 566.0f + (511.0f * 2.0f * 0.08f * 1) + (15.0f * 1), -430.0f, 0.0f });
	m_PowerPotionIcon->AddMaterial(powerPotionMat);
	m_PowerPotionIcon->SetUnitScale(0.08f * 0.7f);
	m_PowerPotionIcon->SetHiddenInGame(true);

	auto healPotionMat = Crystal::CreateShared<Crystal::Material>();
	healPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/potion_heal.tga");
	healPotionMat->bUseAlbedoTextureAlpha = true;

	m_HealPotionIcon = CreateComponent<Crystal::TextureComponent>("HealPotionIconComponent");
	m_HealPotionIcon->SetWorldPosition({ 566.0f + (511.0f * 2.0f * 0.08f * 2) + (15.0f * 2), -430.0f, 0.0f });
	m_HealPotionIcon->AddMaterial(healPotionMat);
	m_HealPotionIcon->SetUnitScale(0.08f * 0.7f);
	m_HealPotionIcon->SetHiddenInGame(true);

	auto shieldPotionMat = Crystal::CreateShared<Crystal::Material>();
	shieldPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/shield_potion.tga");
	shieldPotionMat->bUseAlbedoTextureAlpha = true;

	m_ShieldPotionIcon = CreateComponent<Crystal::TextureComponent>("ShieldPotionIconComponent");
	m_ShieldPotionIcon->SetWorldPosition({ 566.0f + (511.0f * 2.0f * 0.08f * 3) + (15.0f * 3), -430.0f, 0.0f });
	m_ShieldPotionIcon->AddMaterial(shieldPotionMat);
	m_ShieldPotionIcon->SetUnitScale(0.08f * 0.7f);
	m_ShieldPotionIcon->SetHiddenInGame(true);


}

void MyHUD::Update(float deltaTime)
{
	Actor::Update(deltaTime);

	auto mat = m_HitmarkerTextureComponent->GetMaterial(0);
	mat->OpacityMultiplier -= deltaTime * 5.0f;
	mat->OpacityMultiplier = max(0, mat->OpacityMultiplier);
}

void MyHUD::OnPlayerHealthUpdated(float currentHealth, float maxHealth) const
{
	float healthPercent = currentHealth / maxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

	m_HealthFillComponent->SetWorldPosition({-1920.0f / 2.0f + 280.0f - m_HealthBarWidth * 0.35f * (1.0f - healthPercent), -430.0f, 0.0f});
	m_HealthFillComponent->SetScaleX(0.35f * healthPercent);
}

void MyHUD::OnKrakenHealthUpdated(float currentHealth, float maxHealth) const
{
	float healthPercent = currentHealth / maxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

	m_EnemyHpBarFillComponent->SetWorldPosition({ 0.0f - m_HealthBarWidth * 0.7f * (1.0f - healthPercent), 430.0f, 0.0f });
	m_EnemyHpBarFillComponent->SetScaleX(0.7f * healthPercent);
}

void MyHUD::OnPolluteGaugeUpdated(float currentPollute, float maxPolluteGauge) const
{
	float pollutePercent = currentPollute / maxPolluteGauge;
	pollutePercent = std::clamp(pollutePercent, 0.0f, 1.0f);

	m_PollutionBarFillComponent->SetWorldPosition({ 0.0f - m_HealthBarWidth * 0.7f * (1.0f - pollutePercent), 430.0f - m_HealthBarHeight / 2.0f, 0.0f });
	m_PollutionBarFillComponent->SetScaleX(0.7f * pollutePercent);
}

void MyHUD::OnItemAcquired(ItemType itemType) const
{
	switch (itemType)
	{
	case ItemType_Heal:
		m_HealPotionIcon->SetHiddenInGame(false);
		break;
	case ItemType_Power:
		m_PowerPotionIcon->SetHiddenInGame(false);
		break;
	case ItemType_Shield:
		m_ShieldPotionIcon->SetHiddenInGame(false);
		break;
	}
}

void MyHUD::OnItemUsed(ItemType itemType) const
{
	switch (itemType)
	{
	case ItemType_Heal:
		m_HealPotionIcon->SetHiddenInGame(true);
		break;
	case ItemType_Power:
		m_PowerPotionIcon->SetHiddenInGame(true);
		break;
	case ItemType_Shield:
		m_ShieldPotionIcon->SetHiddenInGame(true);
		break;
	}
}

void MyHUD::OnPlayerHitSucceed()
{
	auto mat = m_HitmarkerTextureComponent->GetMaterial(0);
	mat->OpacityMultiplier = 1.0f;
	
}
