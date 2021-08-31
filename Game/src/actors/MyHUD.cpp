#include "MyHUD.h"

#include <DirectXColors.h>

#include "MyPlayerPawn.h"
#include "Crystal/GamePlay/Components/TextRenderingComponent.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/ResourceManager.h"

void MyHUD::Initialize()
{
	DirectX::XMFLOAT2 half = {1920.0f / 2.0f, 1080.0f / 2.0f};

	auto crossHairMat = Crystal::CreateShared<Crystal::Material>();
	crossHairMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/RedCrosshair.tga");
	crossHairMat->bUseAlbedoTextureAlpha = true;

	auto crossHairTexture = CreateComponent<Crystal::TextureComponent>("CrosshairTextureComponent");
	crossHairTexture->AddMaterial(crossHairMat);
	crossHairTexture->SetWorldPosition({half.x, half.y, 0.0f});
	crossHairTexture->SetUnitScale(0.8f);
	crossHairTexture->SetHiddenInGame(false);


	auto hitMarkerMat = Crystal::CreateShared<Crystal::Material>();
	hitMarkerMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hitmarker.png");
	hitMarkerMat->bUseAlbedoTextureAlpha = true;
	hitMarkerMat->Opacity = 0.0f;

	m_HitmarkerTextureComponent = CreateComponent<Crystal::TextureComponent>("HitmarkerTextureComponent");
	m_HitmarkerTextureComponent->AddMaterial(hitMarkerMat);
	crossHairTexture->SetWorldPosition({half.x, half.y, 0.0f});
	m_HitmarkerTextureComponent->SetUnitScale(0.035f);
	m_HitmarkerTextureComponent->SetHiddenInGame(false);



	auto itemBg = Crystal::CreateShared<Crystal::Material>();
	itemBg->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/ItemBg.png");
	itemBg->bUseAlbedoTextureAlpha = true;

	auto itemBg1 = CreateComponent<Crystal::TextureComponent>("itemBg1Component");
	itemBg1->SetWorldPosition({ half.x + 500.0f + (511.0f * 2.0f * 0.1f * 1) + (15.0f * 1), half.y + -430.0f, 0.0f });
	itemBg1->AddMaterial(itemBg);
	itemBg1->SetUnitScale(0.1f);
	itemBg1->SetHiddenInGame(false);

	auto itemBg2 = CreateComponent<Crystal::TextureComponent>("itemBg2Component");
	itemBg2->SetWorldPosition({ half.x + 500.0f + (511.0f * 2.0f * 0.1f * 2) + (15.0f * 2), half.y + -430.0f, 0.0f });
	itemBg2->AddMaterial(itemBg);
	itemBg2->SetUnitScale(0.1f);
	itemBg2->SetHiddenInGame(false);

	auto itemBg3 = CreateComponent<Crystal::TextureComponent>("itemBg3Component");
	itemBg3->SetWorldPosition({ half.x + 500.0f + (511.0f * 2.0f * 0.1f * 3) + (15.0f * 3), half.y + -430.0f, 0.0f });
	itemBg3->AddMaterial(itemBg);
	itemBg3->SetUnitScale(0.1f);
	itemBg3->SetHiddenInGame(false);


	DirectX::XMFLOAT2 itemBgHalf = {
		itemBg->AlbedoTexture.lock()->GetWidth() * itemBg1->GetScale().x, itemBg->AlbedoTexture.lock()->GetHeight() * itemBg1->GetScale().y
	};
	itemBgHalf.x -= 2.0f;
	itemBgHalf.y -= 2.0f;


	auto powerPotionMat = Crystal::CreateShared<Crystal::Material>();
	powerPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/electric_potion.tga");
	powerPotionMat->bUseAlbedoTextureAlpha = true;

	m_PowerPotionIcon = CreateComponent<Crystal::TextureComponent>("PowerPotionIconComponent");
	m_PowerPotionIcon->SetWorldPosition({ half.x + 500.0f + (511.0f * 2.0f * 0.1f * 1) + (15.0f * 1), half.y + -430.0f, 0.0f });
	m_PowerPotionIcon->AddMaterial(powerPotionMat);
	m_PowerPotionIcon->SetUnitScale(0.1f * 0.7f);
	m_PowerPotionIcon->SetHiddenInGame(true);

	auto powerPotionCount = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	powerPotionCount->SetSize(16.0f);
	powerPotionCount->SetText(L"1234");
	auto bg1Position = itemBg1->GetWorldPosition();
	powerPotionCount->SetWorldPosition({ bg1Position.x - itemBgHalf.x, bg1Position.y + itemBgHalf.y, 0.0f });


	auto healPotionMat = Crystal::CreateShared<Crystal::Material>();
	healPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/potion_heal.tga");
	healPotionMat->bUseAlbedoTextureAlpha = true;

	m_HealPotionIcon = CreateComponent<Crystal::TextureComponent>("HealPotionIconComponent");
	m_HealPotionIcon->SetWorldPosition({ half.x + 500.0f + (511.0f * 2.0f * 0.1f * 2) + (15.0f * 2), half.y + -430.0f, 0.0f });
	m_HealPotionIcon->AddMaterial(healPotionMat);
	m_HealPotionIcon->SetUnitScale(0.1f * 0.7f);
	m_HealPotionIcon->SetHiddenInGame(true);

	auto healPotionCount = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	healPotionCount->SetSize(16.0f);
	healPotionCount->SetText(L"1234");
	auto bg2Position = itemBg2->GetWorldPosition();
	healPotionCount->SetWorldPosition({ bg2Position.x - itemBgHalf.x, bg2Position.y + itemBgHalf.y, 0.0f });


	auto shieldPotionMat = Crystal::CreateShared<Crystal::Material>();
	shieldPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/shield_potion.tga");
	shieldPotionMat->bUseAlbedoTextureAlpha = true;

	m_ShieldPotionIcon = CreateComponent<Crystal::TextureComponent>("ShieldPotionIconComponent");
	m_ShieldPotionIcon->SetWorldPosition({ half.x + 500.0f + (511.0f * 2.0f * 0.1f * 3) + (15.0f * 3), half.y + -430.0f, 0.0f });
	m_ShieldPotionIcon->AddMaterial(shieldPotionMat);
	m_ShieldPotionIcon->SetUnitScale(0.1f * 0.7f);
	m_ShieldPotionIcon->SetHiddenInGame(true);

	auto shieldPotionCount = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	shieldPotionCount->SetSize(16.0f);
	shieldPotionCount->SetText(L"1234");
	auto bg3Position = itemBg3->GetWorldPosition();
	shieldPotionCount->SetWorldPosition({ bg3Position.x - itemBgHalf.x, bg3Position.y + itemBgHalf.y, 0.0f });


	auto buttonOutlinedMat = Crystal::CreateShared<Crystal::Material>();
	buttonOutlinedMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_button_outlined.tga");
	buttonOutlinedMat->bUseAlbedoTextureAlpha = true;

	auto checkmarkMat = Crystal::CreateShared<Crystal::Material>();
	checkmarkMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_checmark_2.tga");
	checkmarkMat->bUseAlbedoTextureAlpha = true;


	auto questMainText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	questMainText->SetSize(20.0f);
	questMainText->SetText(L"Destroy Asteroid To Gather Resources");
	questMainText->SetTextColor({ 1.000000000f, 0.843137324f, 0.000000000f });
	questMainText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	questMainText->SetWorldPosition({ 1920.0f * 0.7f, 1080.0f * 0.95f, 0.0f });


	float firstPositionRelative = 0.90f;
	for (int i = 0; i < 3; i++)
	{
		auto buttonOutlined = CreateComponent<Crystal::TextureComponent>("FirstButtonOutlined");
		buttonOutlined->SetWorldPosition({ 1920.0f * 0.7f + 15.0f, 1080.0f * firstPositionRelative, 0.0f });
		buttonOutlined->AddMaterial(buttonOutlinedMat);
		buttonOutlined->SetUnitScale(1 / 60.0f * 15.0f);

		auto checkMark = CreateComponent<Crystal::TextureComponent>("Checkmark");
		checkMark->SetWorldPosition({ 1920.0f * 0.7f + 15.0f, 1080.0f * firstPositionRelative, 0.0f });
		checkMark->AddMaterial(checkmarkMat);
		checkMark->SetUnitScale(1 / 64.0f * 15.0f);


		auto questDetailText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
		questDetailText->SetSize(17.0f);
		questDetailText->SetText(L"Destroy Asteroids");
		questDetailText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
		questDetailText->SetWorldPosition({ 1920.0f * 0.7f + 30.0f + 5.0f, 1080.0f * firstPositionRelative, 0.0f });

		firstPositionRelative -= 0.05f;
	}

	auto bgTextureMat = Crystal::CreateShared<Crystal::Material>();
	bgTextureMat->TintColor = { 0.0f, 0.0f, 0.0f };
	bgTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_bg.tga");
	bgTextureMat->OpacityMultiplier = 0.5f;
	bgTextureMat->bUseAlbedoTextureAlpha = true;

	auto hpFillTextureMat = Crystal::CreateShared<Crystal::Material>();
	hpFillTextureMat->TintColor = { 1.0f, 0.0f, 0.0f };
	hpFillTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_fill.tga");
	hpFillTextureMat->bUseAlbedoTextureAlpha = true;



	m_PlayerBarScale.x = 0.5f;
	m_PlayerBarScale.y = 0.25f;


	m_BarBackgroundWidth = bgTextureMat->AlbedoTexture.lock()->GetWidth();
	m_FillWidth = hpFillTextureMat->AlbedoTexture.lock()->GetWidth();

	m_BarPadding = ((bgTextureMat->AlbedoTexture.lock()->GetHeight() - hpFillTextureMat->AlbedoTexture.lock()->GetHeight()) / 2.0f);
	auto playerFillFinalWidth = (m_BarBackgroundWidth * m_PlayerBarScale.x) - (2.0f * m_BarPadding * m_PlayerBarScale.y);






	auto playerBackgroundTextureComponent = CreateComponent<Crystal::TextureComponent>("Background");
	playerBackgroundTextureComponent->SetWorldPosition({ 300.0f, 1080.0f * 0.95, 1.0f });
	playerBackgroundTextureComponent->AddMaterial(bgTextureMat);
	playerBackgroundTextureComponent->SetScaleX(m_PlayerBarScale.x);
	playerBackgroundTextureComponent->SetScaleY(m_PlayerBarScale.y);

	m_PlayerFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_PlayerFillComponent->SetWorldPosition({ 300.0f - playerFillFinalWidth * (1.0f - 0.5f), 1080.0f * 0.95, 0.0f });
	m_PlayerFillComponent->AddMaterial(hpFillTextureMat);
	m_PlayerFillComponent->SetScaleX(m_PlayerBarScale.x * 1.0f / (m_FillWidth * m_PlayerBarScale.x) * playerFillFinalWidth * 0.5f);
	m_PlayerFillComponent->SetScaleY(m_PlayerBarScale.y);



	m_KrakenBarScale.x = 1.0f;
	m_KrakenBarScale.y = 0.3f;


	auto krakenFillFinalWidth = (m_BarBackgroundWidth * m_KrakenBarScale.x) - (2.0f * m_BarPadding * m_KrakenBarScale.y);

	auto krakenBackgroundTextureComponent = CreateComponent<Crystal::TextureComponent>("Background");
	krakenBackgroundTextureComponent->SetWorldPosition({ 1920.0f / 2.0f, 1080.0f * 0.20f, 1.0f });
	krakenBackgroundTextureComponent->AddMaterial(bgTextureMat);
	krakenBackgroundTextureComponent->SetScaleX(m_KrakenBarScale.x);
	krakenBackgroundTextureComponent->SetScaleY(m_KrakenBarScale.y);

	m_KrakenFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_KrakenFillComponent->SetWorldPosition({ 1920.0f / 2.0f - krakenFillFinalWidth * (1.0f - 0.5f), 1080.0f * 0.20f, 0.0f });
	m_KrakenFillComponent->AddMaterial(hpFillTextureMat);
	m_KrakenFillComponent->SetScaleX(m_KrakenBarScale.x * 1.0f / (m_FillWidth * m_KrakenBarScale.x) * krakenFillFinalWidth * 0.5f);
	m_KrakenFillComponent->SetScaleY(m_KrakenBarScale.y);

	auto krakenBarText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	krakenBarText->SetSize(15.0f);
	krakenBarText->SetText(L"Space Calamity, Kraken");
	krakenBarText->SetTextColor(Crystal::Vector3::White);
	krakenBarText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	krakenBarText->SetWorldPosition({ 1920.0f / 2.0f - krakenFillFinalWidth, 1080.0f * 0.20f + 17.0f, 0.0f });




	auto polluteFillTextureMat = Crystal::CreateShared<Crystal::Material>();
	polluteFillTextureMat->TintColor = { 30.0f / 255.0f, 100.0f / 255.0f, 30.0f / 255.0f };
	polluteFillTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_fill.tga");
	polluteFillTextureMat->bUseAlbedoTextureAlpha = true;


	m_PolluteBarScale.x = 1.0f;
	m_PolluteBarScale.y = 0.3f;

	auto polluteFillFinalWidth = (m_BarBackgroundWidth * m_PolluteBarScale.x) - (2.0f * m_BarPadding * m_PolluteBarScale.y);

	auto polluteBackgroundFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	polluteBackgroundFillComponent->SetWorldPosition({ 1920.0f / 2.0f, 1080.0f * 0.15f, 1.0f });
	polluteBackgroundFillComponent->AddMaterial(bgTextureMat);
	polluteBackgroundFillComponent->SetScaleX(m_PolluteBarScale.x);
	polluteBackgroundFillComponent->SetScaleY(m_PolluteBarScale.y);

	m_PolluteFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_PolluteFillComponent->SetWorldPosition({ 1920.0f / 2.0f - polluteFillFinalWidth * (1.0f - 0.5f), 1080.0f * 0.15f, 0.0f });
	m_PolluteFillComponent->AddMaterial(polluteFillTextureMat);
	m_PolluteFillComponent->SetScaleX(m_PolluteBarScale.x * 1.0f / (m_FillWidth * m_PolluteBarScale.x) * polluteFillFinalWidth * 0.5f);
	m_PolluteFillComponent->SetScaleY(m_PolluteBarScale.y);

	auto polluteBarText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	polluteBarText->SetSize(15.0f);
	polluteBarText->SetText(L"Space Polllution Level");
	polluteBarText->SetTextColor(Crystal::Vector3::White);
	polluteBarText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	polluteBarText->SetWorldPosition({ 1920.0f / 2.0f - polluteFillFinalWidth, 1080.0f * 0.15f + 17.0f , 0.0f });


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
	float percent = currentHealth / maxHealth;
	percent = std::clamp(percent, 0.0f, 1.0f);


	auto playerFillFinalWidth = (m_BarBackgroundWidth * m_PlayerBarScale.x) - (2.0f * m_BarPadding * m_PlayerBarScale.y);

	m_PlayerFillComponent->SetWorldPosition({ 300.0f - playerFillFinalWidth * (1.0f - percent), 1080.0f * 0.95, 0.0f });
	m_PlayerFillComponent->SetScaleX(m_PlayerBarScale.x * 1 / (m_FillWidth * m_PlayerBarScale.x) * playerFillFinalWidth * percent);
}

void MyHUD::OnKrakenHealthUpdated(float currentHealth, float maxHealth) const
{
	float healthPercent = currentHealth / maxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

	auto krakenFillFinalWidth = (m_BarBackgroundWidth * m_KrakenBarScale.x) - (2.0f * m_BarPadding * m_KrakenBarScale.y);

	m_KrakenFillComponent->SetWorldPosition({ 1920.0f / 2.0f - krakenFillFinalWidth * (1.0f - healthPercent), 1080.0f * 0.20f, 0.0f });
	m_KrakenFillComponent->SetScaleX(m_KrakenBarScale.x * 1.0f / (m_FillWidth * m_KrakenBarScale.x) * krakenFillFinalWidth * healthPercent);
}

void MyHUD::OnPolluteGaugeUpdated(float currentPollute, float maxPolluteGauge) const
{
	float pollutePercent = currentPollute / maxPolluteGauge;
	pollutePercent = std::clamp(pollutePercent, 0.0f, 1.0f);

	auto polluteFillFinalWidth = (m_BarBackgroundWidth * m_PolluteBarScale.x) - (2.0f * m_BarPadding * m_PolluteBarScale.y);
	m_PolluteFillComponent->SetWorldPosition({ 1920.0f / 2.0f - polluteFillFinalWidth * (1.0f - pollutePercent), 1080.0f * 0.15f, 0.0f });
	m_PolluteFillComponent->SetScaleX(m_PolluteBarScale.x * 1.0f / (m_FillWidth * m_PolluteBarScale.x) * polluteFillFinalWidth * pollutePercent);
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
