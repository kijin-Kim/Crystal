#pragma once
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "SpaceCombatTypes.h"

class MyHUD : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
		ar & m_HealthFillComponent;
		ar & m_PowerPotionIcon;
		ar & m_HealPotionIcon;
		ar & m_ShieldPotionIcon;
	}

public:
	MyHUD() = default;
	~MyHUD() override = default;

	void Initialize() override
	{
		auto crossHairMat = Crystal::CreateShared<Crystal::Material>();
		crossHairMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/RedCrosshair.tga");

		auto crossHairTexture = CreateComponent<Crystal::TextureComponent>("CrosshairTextureComponent");
		crossHairTexture->AddMaterial(crossHairMat);
		crossHairTexture->SetUnitScale(0.8f);


		auto barBgMat = Crystal::CreateShared<Crystal::Material>();
		barBgMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarBg.png");

		auto hpBarBg = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
		hpBarBg->AddMaterial(barBgMat);
		hpBarBg->SetUnitScale(0.35f);
		hpBarBg->SetScaleY(0.13f);
		hpBarBg->SetWorldPosition({-1920.0f / 2.0f + 280.0f, -430.0f, 1.0f});


		auto hpBarFillMat = Crystal::CreateShared<Crystal::Material>();
		hpBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarFill.png");

		m_HealthBarWidth = hpBarFillMat->AlbedoTexture.lock()->GetWidth();
		m_HealthBarHeight = hpBarFillMat->AlbedoTexture.lock()->GetHeight();

		m_HealthFillComponent = CreateComponent<Crystal::TextureComponent>("HpFillTextureComponent");
		m_HealthFillComponent->SetWorldPosition({-1920.0f / 2.0f + 280.0f - m_HealthBarWidth * 0.35f * 0.5f, -430.0f, 0.0f});
		m_HealthFillComponent->AddMaterial(hpBarFillMat);
		m_HealthFillComponent->SetUnitScale(0.35f);
		m_HealthFillComponent->SetScaleX(0.35f * 0.5f);
		m_HealthFillComponent->SetScaleY(0.13f);



		auto enemyHpBarBg = CreateComponent<Crystal::TextureComponent>("EnemyHpBackgroundTextureComponent");
		enemyHpBarBg->AddMaterial(barBgMat);
		enemyHpBarBg->SetUnitScale(0.7f);
		enemyHpBarBg->SetScaleY(0.20f);
		enemyHpBarBg->SetWorldPosition({ 0.0f, 430.0f, 1.0f });

		m_EnemyHpBarFillComponent = CreateComponent<Crystal::TextureComponent>("EnemyHpFillTextureComponent");
		m_EnemyHpBarFillComponent->SetWorldPosition({ 0.0f - m_HealthBarWidth * 0.7f * 0.5f, 430.0f, 0.0f });
		m_EnemyHpBarFillComponent->AddMaterial(hpBarFillMat);
		m_EnemyHpBarFillComponent->SetUnitScale(0.7f);
		m_EnemyHpBarFillComponent->SetScaleX(0.7f * 0.5f);
		m_EnemyHpBarFillComponent->SetScaleY(0.20f);

		
		auto pollutionHpBarBg = CreateComponent<Crystal::TextureComponent>("PollutionBackgroundTextureComponent");
		pollutionHpBarBg->AddMaterial(barBgMat);
		pollutionHpBarBg->SetUnitScale(0.7f);
		pollutionHpBarBg->SetScaleY(0.10f);
		pollutionHpBarBg->SetWorldPosition({ 0.0f, 430.0f - m_HealthBarHeight / 2.0f, 1.0f });

		auto pollutionBarFillMat = Crystal::CreateShared<Crystal::Material>();
		pollutionBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/PollutionBarFill.png");

		m_PollutionBarFillComponent = CreateComponent<Crystal::TextureComponent>("PollutionFillTextureComponent");
		m_PollutionBarFillComponent->SetWorldPosition({ 0.0f - m_HealthBarWidth * 0.7f * 0.5f, 430.0f - m_HealthBarHeight / 2.0f, 0.0f });
		m_PollutionBarFillComponent->AddMaterial(pollutionBarFillMat);
		m_PollutionBarFillComponent->SetUnitScale(0.7f);
		m_PollutionBarFillComponent->SetScaleX(0.7f * 0.0f);
		m_PollutionBarFillComponent->SetScaleY(0.10f);
		




		auto itemBg = Crystal::CreateShared<Crystal::Material>();
		itemBg->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/ItemBg.png");

		auto itemBg1 = CreateComponent<Crystal::TextureComponent>("itemBg1Component");
		itemBg1->SetWorldPosition({566.0f, -430.0f, 0.0f});
		itemBg1->AddMaterial(itemBg);
		itemBg1->SetUnitScale(0.08f);

		auto itemBg2 = CreateComponent<Crystal::TextureComponent>("itemBg2Component");
		itemBg2->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 1) + (15.0f * 1), -430.0f, 0.0f});
		itemBg2->AddMaterial(itemBg);
		itemBg2->SetUnitScale(0.08f);

		auto itemBg3 = CreateComponent<Crystal::TextureComponent>("itemBg3Component");
		itemBg3->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 2) + (15.0f * 2), -430.0f, 0.0f});
		itemBg3->AddMaterial(itemBg);
		itemBg3->SetUnitScale(0.08f);

		auto itemBg4 = CreateComponent<Crystal::TextureComponent>("itemBg4Component");
		itemBg4->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 3) + (15.0f * 3), -430.0f, 0.0f});
		itemBg4->AddMaterial(itemBg);
		itemBg4->SetUnitScale(0.08f);


		auto powerPotionMat = Crystal::CreateShared<Crystal::Material>();
		powerPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/electric_potion.tga");

		m_PowerPotionIcon = CreateComponent<Crystal::TextureComponent>("PowerPotionIconComponent");
		m_PowerPotionIcon->SetWorldPosition({ 566.0f, -430.0f, 0.0f });
		m_PowerPotionIcon->AddMaterial(powerPotionMat);
		m_PowerPotionIcon->SetUnitScale(0.08f * 0.7f);
		m_PowerPotionIcon->SetHideInGame(true);

		auto healPotionMat = Crystal::CreateShared<Crystal::Material>();
		healPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/potion_heal.tga");

		m_HealPotionIcon = CreateComponent<Crystal::TextureComponent>("HealPotionIconComponent");
		m_HealPotionIcon->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 1) + (15.0f * 1), -430.0f, 0.0f});
		m_HealPotionIcon->AddMaterial(healPotionMat);
		m_HealPotionIcon->SetUnitScale(0.08f * 0.7f);
		m_HealPotionIcon->SetHideInGame(true);

		auto shieldPotionMat = Crystal::CreateShared<Crystal::Material>();
		shieldPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/shield_potion.tga");

		m_ShieldPotionIcon = CreateComponent<Crystal::TextureComponent>("ShieldPotionIconComponent");
		m_ShieldPotionIcon->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 2) + (15.0f * 2), -430.0f, 0.0f});
		m_ShieldPotionIcon->AddMaterial(shieldPotionMat);
		m_ShieldPotionIcon->SetUnitScale(0.08f * 0.7f);
		m_ShieldPotionIcon->SetHideInGame(true);

		auto bulletMat = Crystal::CreateShared<Crystal::Material>();
		bulletMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/bullets.tga");

		auto bulletIcon = CreateComponent<Crystal::TextureComponent>("BulletIconComponent");
		bulletIcon->SetWorldPosition({566.0f + (511.0f * 2.0f * 0.08f * 3) + (15.0f * 3), -430.0f, 0.0f});
		bulletIcon->AddMaterial(bulletMat);
		bulletIcon->SetUnitScale(0.08f * 0.7f);
	}

	void OnPlayerHealthUpdated(float currentHealth, float maxHealth)
	{
		float healthPercent = currentHealth / maxHealth;
		healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

		m_HealthFillComponent->SetWorldPosition({-1920.0f / 2.0f + 280.0f - m_HealthBarWidth * 0.35f * (1.0f - healthPercent), -430.0f, 0.0f});
		m_HealthFillComponent->SetScaleX(0.35f * healthPercent);
	}

	void OnKrakenHealthUpdated(float currentHealth, float maxHealth)
	{
		float healthPercent = currentHealth / maxHealth;
		healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

		m_EnemyHpBarFillComponent->SetWorldPosition({ 0.0f - m_HealthBarWidth * 0.7f * (1.0f - healthPercent), 430.0f, 0.0f });
		m_EnemyHpBarFillComponent->SetScaleX(0.7f * healthPercent);
	}

	float GetPolluteGauge() const { return m_CurrentPolluteGauge; }
	void SetPolluteGague(float polluteGauge)
	{
		m_CurrentPolluteGauge = polluteGauge;
		OnPolluteGaugeUpdated(m_CurrentPolluteGauge);
	}

	void OnPolluteGaugeUpdated(float currentPollute)
	{
		float pollutePercent = currentPollute / m_MaxPolluteGauge;
		pollutePercent = std::clamp(pollutePercent, 0.0f, 1.0f);

		m_PollutionBarFillComponent->SetWorldPosition({ 0.0f - m_HealthBarWidth * 0.7f * (1.0f - pollutePercent), 430.0f - m_HealthBarHeight / 2.0f, 0.0f });
		m_PollutionBarFillComponent->SetScaleX(0.7f * pollutePercent);
	}

	void OnItemAcquired(ItemType itemType)
	{
		switch (itemType)
		{
		case ItemType_Heal:
			m_HealPotionIcon->SetHideInGame(false);
			break;
		case ItemType_Power:
			m_PowerPotionIcon->SetHideInGame(false);
			break;
		case ItemType_Shield:
			m_ShieldPotionIcon->SetHideInGame(false);
			break;
		}
	}

	void OnItemUsed(ItemType itemType)
	{
		switch (itemType)
		{
		case ItemType_Heal:
			m_HealPotionIcon->SetHideInGame(true);
			break;
		case ItemType_Power:
			m_PowerPotionIcon->SetHideInGame(true);
			break;
		case ItemType_Shield:
			m_ShieldPotionIcon->SetHideInGame(true);
			break;
		}
	}
	


	STATIC_TYPE_IMPLE(MyHUD)

private:
	Crystal::Shared<Crystal::TextureComponent> m_HealthFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_EnemyHpBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_PollutionBarFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_PowerPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_ShieldPotionIcon = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;

	float m_CurrentPolluteGauge = 0.0f;
	float m_MaxPolluteGauge = 300.0f;
};
