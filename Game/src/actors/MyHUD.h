#pragma once
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

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
		auto crossHairMat = std::make_unique<Crystal::Material>();
		crossHairMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/RedCrosshair.tga");

		auto crossHairTexture = CreateComponent<Crystal::TextureComponent>("CrosshairTextureComponent");
		crossHairTexture->AddMaterial(std::move(crossHairMat));
		crossHairTexture->SetUnitScale(0.8f);


		auto hpBarBgMat = std::make_unique<Crystal::Material>();
		hpBarBgMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarBg.png");

		auto hpBarBg = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
		hpBarBg->AddMaterial(std::move(hpBarBgMat));
		hpBarBg->SetUnitScale(0.35f);
		hpBarBg->SetScaleY(0.18f);
		hpBarBg->SetWorldPosition({-1920.0f / 2.0f + 280.0f, -430.0f, 1.0f});


		auto hpBarFillMat = std::make_unique<Crystal::Material>();
		hpBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarFill.png");

		m_HealthBarWidth = hpBarFillMat->AlbedoTexture.lock()->GetWidth();

		m_HealthFillComponent = CreateComponent<Crystal::TextureComponent>("HpFillTextureComponent");
		m_HealthFillComponent->SetWorldPosition({-1920.0f / 2.0f + 280.0f - m_HealthBarWidth * 0.35f * 0.5f, -430.0f, 0.0f});
		m_HealthFillComponent->AddMaterial(std::move(hpBarFillMat));
		m_HealthFillComponent->SetUnitScale(0.35f);
		m_HealthFillComponent->SetScaleX(0.35f * 0.5f);
		m_HealthFillComponent->SetScaleY(0.18f);


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

	void OnHealthUpdated(int currentHealth, int maxHealth)
	{
		float healthPercent = (float)currentHealth / (float)maxHealth;


		m_HealthFillComponent->SetWorldPosition({-1920.0f / 2.0f + 280.0f - m_HealthBarWidth * 0.35f * (1.0f - healthPercent), -430.0f, 0.0f});
		m_HealthFillComponent->SetScaleX(0.35f * healthPercent);
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
	Crystal::Shared<Crystal::TextureComponent> m_PowerPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_ShieldPotionIcon = nullptr;
	int m_HealthBarWidth = 0;
};
