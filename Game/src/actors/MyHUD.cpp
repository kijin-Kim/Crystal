#include "MyHUD.h"

#include <DirectXColors.h>

#include "MyPlayerPawn.h"
#include "Quest.h"
#include "Crystal/GamePlay/Components/ButtonComponent.h"
#include "Crystal/GamePlay/Components/TextRenderingComponent.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/Gameplay/Controllers/PlayerController.h"
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
	crossHairTexture->SetWorldPosition({half.x, half.y, 4.0f});
	crossHairTexture->SetUnitScale(0.8f);
	crossHairTexture->SetHiddenInGame(false);


	auto hitMarkerMat = Crystal::CreateShared<Crystal::Material>();
	hitMarkerMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hitmarker.png");
	hitMarkerMat->bUseAlbedoTextureAlpha = true;
	hitMarkerMat->Opacity = 0.0f;

	m_HitmarkerTextureComponent = CreateComponent<Crystal::TextureComponent>("HitmarkerTextureComponent");
	m_HitmarkerTextureComponent->AddMaterial(hitMarkerMat);
	m_HitmarkerTextureComponent->SetWorldPosition({half.x, half.y, 4.0f});
	m_HitmarkerTextureComponent->SetUnitScale(0.035f);
	m_HitmarkerTextureComponent->SetHiddenInGame(false);


	auto hoveredMat = Crystal::CreateShared<Crystal::Material>();
	hoveredMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/selected.png");
	hoveredMat->bUseAlbedoTextureAlpha = true;


	m_HoveredTexture = CreateComponent<Crystal::TextureComponent>("HoveredTexture");
	m_HoveredTexture->AddMaterial(hoveredMat);
	m_HoveredTexture->SetWorldPosition({half.x, half.y, 3.0f});
	m_HoveredTextureMaxScale = 1.0f / 512.0f * 80.0f;
	m_HoveredTextureWidth = 512.0f;
	m_HoveredTexture->SetUnitScale(m_HoveredTextureMaxScale);
	m_HoveredTexture->SetHiddenInGame(true);


	auto hoveredDetailBackgroundMat = Crystal::CreateShared<Crystal::Material>();
	hoveredDetailBackgroundMat->AlbedoColor = {1.0f, 0.0f, 0.0f};
	hoveredDetailBackgroundMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/bw_gradient.png");
	hoveredDetailBackgroundMat->OpacityMultiplier = 0.5f;
	hoveredDetailBackgroundMat->bUseAlbedoTextureAlpha = false;


	m_HoveredDetailBackgroundTexture = CreateComponent<Crystal::TextureComponent>("HoveredDetailBackground");
	m_HoveredDetailBackgroundTexture->AddMaterial(hoveredDetailBackgroundMat);
	m_HoveredDetailBackgroundTexture->SetWorldPosition({half.x, half.y, 0.0});
	m_HoveredDetailBackgroundTexture->SetUnitScale(0.2f);
	m_HoveredDetailBackgroundTexture->SetHiddenInGame(true);


	m_HoveredDetailNameText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_HoveredDetailNameText->SetSize(20.0f);
	m_HoveredDetailNameText->SetText(L"Name");
	m_HoveredDetailNameText->SetWorldPosition({half.x, half.y, 1.0f});
	m_HoveredDetailNameText->SetHiddenInGame(true);

	m_HoveredDetailDistanceText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_HoveredDetailDistanceText->SetSize(20.0f);
	m_HoveredDetailDistanceText->SetText(L"100km");
	m_HoveredDetailDistanceText->SetWorldPosition({half.x, half.y, 1.0f});
	m_HoveredDetailDistanceText->SetHiddenInGame(true);


	m_HoveredDetailedBackgroundTextureWidth = hoveredDetailBackgroundMat->OpacityTexture.lock()->GetWidth() * 0.2f;
	m_HoveredDetailedBackgroundTextureHeight = hoveredDetailBackgroundMat->OpacityTexture.lock()->GetHeight() * 0.2f;

	auto itemBg = Crystal::CreateShared<Crystal::Material>();
	itemBg->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/ItemBg.png");
	itemBg->bUseAlbedoTextureAlpha = true;

	auto itemBg1 = CreateComponent<Crystal::TextureComponent>("itemBg1Component");
	itemBg1->SetWorldPosition({half.x + 500.0f + (511.0f * 2.0f * 0.1f * 1) + (15.0f * 1), half.y + -430.0f, 0.0f});
	itemBg1->AddMaterial(itemBg);
	itemBg1->SetUnitScale(0.1f);
	itemBg1->SetHiddenInGame(false);

	auto itemBg2 = CreateComponent<Crystal::TextureComponent>("itemBg2Component");
	itemBg2->SetWorldPosition({half.x + 500.0f + (511.0f * 2.0f * 0.1f * 2) + (15.0f * 2), half.y + -430.0f, 0.0f});
	itemBg2->AddMaterial(itemBg);
	itemBg2->SetUnitScale(0.1f);
	itemBg2->SetHiddenInGame(false);

	auto itemBg3 = CreateComponent<Crystal::TextureComponent>("itemBg3Component");
	itemBg3->SetWorldPosition({half.x + 500.0f + (511.0f * 2.0f * 0.1f * 3) + (15.0f * 3), half.y + -430.0f, 0.0f});
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
	m_PowerPotionIcon->SetWorldPosition({half.x + 500.0f + (511.0f * 2.0f * 0.1f * 1) + (15.0f * 1), half.y + -430.0f, 0.0f});
	m_PowerPotionIcon->AddMaterial(powerPotionMat);
	m_PowerPotionIcon->SetUnitScale(0.1f * 0.7f);
	m_PowerPotionIcon->SetHiddenInGame(true);

	m_PowerPotionCount = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_PowerPotionCount->SetSize(16.0f);
	m_PowerPotionCount->SetText(L"1234");
	auto bg1Position = itemBg1->GetWorldPosition();
	m_PowerPotionCount->SetWorldPosition({bg1Position.x - itemBgHalf.x, bg1Position.y + itemBgHalf.y, 0.0f});


	auto healPotionMat = Crystal::CreateShared<Crystal::Material>();
	healPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/potion_heal.tga");
	healPotionMat->bUseAlbedoTextureAlpha = true;

	m_HealPotionIcon = CreateComponent<Crystal::TextureComponent>("HealPotionIconComponent");
	m_HealPotionIcon->SetWorldPosition({half.x + 500.0f + (511.0f * 2.0f * 0.1f * 2) + (15.0f * 2), half.y + -430.0f, 0.0f});
	m_HealPotionIcon->AddMaterial(healPotionMat);
	m_HealPotionIcon->SetUnitScale(0.1f * 0.7f);
	m_HealPotionIcon->SetHiddenInGame(true);

	m_HealPotionCount = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_HealPotionCount->SetSize(16.0f);
	m_HealPotionCount->SetText(L"1234");
	auto bg2Position = itemBg2->GetWorldPosition();
	m_HealPotionCount->SetWorldPosition({bg2Position.x - itemBgHalf.x, bg2Position.y + itemBgHalf.y, 0.0f});


	auto shieldPotionMat = Crystal::CreateShared<Crystal::Material>();
	shieldPotionMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/shield_potion.tga");
	shieldPotionMat->bUseAlbedoTextureAlpha = true;

	m_ShieldPotionIcon = CreateComponent<Crystal::TextureComponent>("ShieldPotionIconComponent");
	m_ShieldPotionIcon->SetWorldPosition({half.x + 500.0f + (511.0f * 2.0f * 0.1f * 3) + (15.0f * 3), half.y + -430.0f, 0.0f});
	m_ShieldPotionIcon->AddMaterial(shieldPotionMat);
	m_ShieldPotionIcon->SetUnitScale(0.1f * 0.7f);
	m_ShieldPotionIcon->SetHiddenInGame(true);

	m_ShieldPotionCount = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_ShieldPotionCount->SetSize(16.0f);
	m_ShieldPotionCount->SetText(L"1234");
	auto bg3Position = itemBg3->GetWorldPosition();
	m_ShieldPotionCount->SetWorldPosition({bg3Position.x - itemBgHalf.x, bg3Position.y + itemBgHalf.y, 0.0f});


	auto buttonOutlinedMat = Crystal::CreateShared<Crystal::Material>();
	buttonOutlinedMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_button_outlined.tga");
	buttonOutlinedMat->bUseAlbedoTextureAlpha = true;

	auto checkmarkMat = Crystal::CreateShared<Crystal::Material>();
	checkmarkMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_checmark_2.tga");
	checkmarkMat->bUseAlbedoTextureAlpha = true;


	m_QuestMainText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_QuestMainText->SetSize(20.0f);
	m_QuestMainText->SetText(L"Destroy Asteroid To Gather Resources");
	m_QuestMainText->SetTextColor({1.000000000f, 0.843137324f, 0.000000000f});
	m_QuestMainText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_QuestMainText->SetWorldPosition({1920.0f * 0.7f, 1080.0f * 0.95f, 0.0f});
	m_QuestMainText->SetHiddenInGame(true);


	float firstPositionRelative = 0.90f;

	for (int i = 0; i < 5; i++)
	{
		auto buttonOutlined = CreateComponent<Crystal::TextureComponent>("FirstButtonOutlined");
		buttonOutlined->SetWorldPosition({1920.0f * 0.7f + 15.0f, 1080.0f * firstPositionRelative, 0.0f});
		buttonOutlined->AddMaterial(buttonOutlinedMat);
		buttonOutlined->SetUnitScale(1 / 60.0f * 15.0f);
		buttonOutlined->SetHiddenInGame(true);
		m_QuestCompletedMarkerOutlines.push_back(buttonOutlined);

		auto checkMark = CreateComponent<Crystal::TextureComponent>("Checkmark");
		checkMark->SetWorldPosition({1920.0f * 0.7f + 15.0f, 1080.0f * firstPositionRelative, 0.0f});
		checkMark->AddMaterial(checkmarkMat);
		checkMark->SetUnitScale(1 / 64.0f * 15.0f);
		checkMark->SetHiddenInGame(true);
		m_QuestCompletedMarkers.push_back(checkMark);


		auto questDetailText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
		questDetailText->SetSize(17.0f);
		questDetailText->SetText(L"Destroy Asteroids");
		questDetailText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
		questDetailText->SetWorldPosition({1920.0f * 0.7f + 30.0f + 5.0f, 1080.0f * firstPositionRelative, 0.0f});
		questDetailText->SetHiddenInGame(true);
		m_QuestDisplayTexts.push_back(questDetailText);

		firstPositionRelative -= 0.05f;
	}

	auto bgTextureMat = Crystal::CreateShared<Crystal::Material>();
	bgTextureMat->TintColor = {0.0f, 0.0f, 0.0f};
	bgTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_bg.tga");
	bgTextureMat->OpacityMultiplier = 0.5f;
	bgTextureMat->bUseAlbedoTextureAlpha = true;

	auto hpFillTextureMat = Crystal::CreateShared<Crystal::Material>();
	hpFillTextureMat->TintColor = {1.0f, 0.0f, 0.0f};
	hpFillTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_fill.tga");
	hpFillTextureMat->bUseAlbedoTextureAlpha = true;


	m_PlayerBarScale.x = 0.5f;
	m_PlayerBarScale.y = 0.25f;


	m_BarBackgroundWidth = bgTextureMat->AlbedoTexture.lock()->GetWidth();
	m_FillWidth = hpFillTextureMat->AlbedoTexture.lock()->GetWidth();

	m_BarPadding = ((bgTextureMat->AlbedoTexture.lock()->GetHeight() - hpFillTextureMat->AlbedoTexture.lock()->GetHeight()) / 2.0f);
	auto playerFillFinalWidth = (m_BarBackgroundWidth * m_PlayerBarScale.x) - (2.0f * m_BarPadding * m_PlayerBarScale.y);


	auto playerBackgroundTextureComponent = CreateComponent<Crystal::TextureComponent>("Background");
	playerBackgroundTextureComponent->SetWorldPosition({300.0f, 1080.0f * 0.95, 0.0f});
	playerBackgroundTextureComponent->AddMaterial(bgTextureMat);
	playerBackgroundTextureComponent->SetScaleX(m_PlayerBarScale.x);
	playerBackgroundTextureComponent->SetScaleY(m_PlayerBarScale.y);

	m_PlayerFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_PlayerFillComponent->SetWorldPosition({300.0f - playerFillFinalWidth * (1.0f - 0.5f), 1080.0f * 0.95, 1.0f});
	m_PlayerFillComponent->AddMaterial(hpFillTextureMat);
	m_PlayerFillComponent->SetScaleX(m_PlayerBarScale.x * 1.0f / (m_FillWidth * m_PlayerBarScale.x) * playerFillFinalWidth * 0.5f);
	m_PlayerFillComponent->SetScaleY(m_PlayerBarScale.y);


	m_KrakenBarScale.x = 1.0f;
	m_KrakenBarScale.y = 0.3f;

	m_CruiserBarScale.x = 1.0f;
	m_CruiserBarScale.y = 0.3f;

	m_SpaceStationBarScale.x = 1.0f;
	m_SpaceStationBarScale.y = 0.3f;


	auto krakenFillFinalWidth = (m_BarBackgroundWidth * m_KrakenBarScale.x) - (2.0f * m_BarPadding * m_KrakenBarScale.y);

	m_KrakenBackgroundComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_KrakenBackgroundComponent->SetWorldPosition({1920.0f / 2.0f, 1080.0f * 0.20f, 0.0f});
	m_KrakenBackgroundComponent->AddMaterial(bgTextureMat);
	m_KrakenBackgroundComponent->SetScaleX(m_KrakenBarScale.x);
	m_KrakenBackgroundComponent->SetScaleY(m_KrakenBarScale.y);

	m_KrakenFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_KrakenFillComponent->SetWorldPosition({1920.0f / 2.0f - krakenFillFinalWidth * (1.0f - 0.5f), 1080.0f * 0.20f, 1.0f});
	m_KrakenFillComponent->AddMaterial(hpFillTextureMat);
	m_KrakenFillComponent->SetScaleX(m_KrakenBarScale.x * 1.0f / (m_FillWidth * m_KrakenBarScale.x) * krakenFillFinalWidth * 0.5f);
	m_KrakenFillComponent->SetScaleY(m_KrakenBarScale.y);

	m_KrakenBarText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_KrakenBarText->SetSize(15.0f);
	m_KrakenBarText->SetText(L"Space Calamity, Kraken");
	m_KrakenBarText->SetTextColor(Crystal::Vector3::White);
	m_KrakenBarText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_KrakenBarText->SetWorldPosition({1920.0f / 2.0f - krakenFillFinalWidth, 1080.0f * 0.20f + 17.0f, 0.0f});


	auto cruiserFillMat = Crystal::CreateShared<Crystal::Material>();
	cruiserFillMat->TintColor = { 0.0f, 1.0f, 0.0f };
	cruiserFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_fill.tga");
	cruiserFillMat->bUseAlbedoTextureAlpha = true;

	m_CruiserBackgroundComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_CruiserBackgroundComponent->SetWorldPosition({ 1920.0f / 2.0f, 1080.0f * 0.20f, 0.0f });
	m_CruiserBackgroundComponent->AddMaterial(bgTextureMat);
	m_CruiserBackgroundComponent->SetScaleX(m_CruiserBarScale.x);
	m_CruiserBackgroundComponent->SetScaleY(m_CruiserBarScale.y);

	m_CruiserFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_CruiserFillComponent->SetWorldPosition({ 1920.0f / 2.0f - krakenFillFinalWidth * (1.0f - 0.5f), 1080.0f * 0.20f, 1.0f });
	m_CruiserFillComponent->AddMaterial(cruiserFillMat);
	m_CruiserFillComponent->SetScaleX(m_CruiserBarScale.x * 1.0f / (m_FillWidth * m_CruiserBarScale.x) * krakenFillFinalWidth * 0.5f);
	m_CruiserFillComponent->SetScaleY(m_CruiserBarScale.y);

	m_CruiserBarText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_CruiserBarText->SetSize(15.0f);
	m_CruiserBarText->SetText(L"Ally Cruiser");
	m_CruiserBarText->SetTextColor(Crystal::Vector3::White);
	m_CruiserBarText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_CruiserBarText->SetWorldPosition({ 1920.0f / 2.0f - krakenFillFinalWidth, 1080.0f * 0.20f + 17.0f, 0.0f });



	auto spaceStationFillMat = Crystal::CreateShared<Crystal::Material>();
	spaceStationFillMat->TintColor = { 0.0f, 1.0f, 0.0f };
	spaceStationFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_fill.tga");

	m_SpaceStationBackgroundComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_SpaceStationBackgroundComponent->SetWorldPosition({ 1920.0f / 2.0f, 1080.0f * 0.20f, 0.0f });
	m_SpaceStationBackgroundComponent->AddMaterial(bgTextureMat);
	m_SpaceStationBackgroundComponent->SetScaleX(m_SpaceStationBarScale.x);
	m_SpaceStationBackgroundComponent->SetScaleY(m_SpaceStationBarScale.y);

	m_SpaceStationFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_SpaceStationFillComponent->SetWorldPosition({ 1920.0f / 2.0f - krakenFillFinalWidth * (1.0f - 0.5f), 1080.0f * 0.20f, 1.0f });
	m_SpaceStationFillComponent->AddMaterial(spaceStationFillMat);
	m_SpaceStationFillComponent->SetScaleX(m_SpaceStationBarScale.x * 1.0f / (m_FillWidth * m_SpaceStationBarScale.x) * krakenFillFinalWidth * 0.5f);
	m_SpaceStationFillComponent->SetScaleY(m_SpaceStationBarScale.y);

	m_SpaceStationBarText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_SpaceStationBarText->SetSize(15.0f);
	m_SpaceStationBarText->SetText(L"SpaceStation");
	m_SpaceStationBarText->SetTextColor(Crystal::Vector3::White);
	m_SpaceStationBarText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_SpaceStationBarText->SetWorldPosition({ 1920.0f / 2.0f - krakenFillFinalWidth, 1080.0f * 0.20f + 17.0f, 0.0f });




	auto polluteFillTextureMat = Crystal::CreateShared<Crystal::Material>();
	polluteFillTextureMat->TintColor = {30.0f / 255.0f, 100.0f / 255.0f, 30.0f / 255.0f};
	polluteFillTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_fill.tga");
	polluteFillTextureMat->bUseAlbedoTextureAlpha = true;


	m_PolluteBarScale.x = 1.0f;
	m_PolluteBarScale.y = 0.3f;

	auto polluteFillFinalWidth = (m_BarBackgroundWidth * m_PolluteBarScale.x) - (2.0f * m_BarPadding * m_PolluteBarScale.y);

	m_PolluteBackgroundComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_PolluteBackgroundComponent->SetWorldPosition({1920.0f / 2.0f, 1080.0f * 0.15f, 0.0f});
	m_PolluteBackgroundComponent->AddMaterial(bgTextureMat);
	m_PolluteBackgroundComponent->SetScaleX(m_PolluteBarScale.x);
	m_PolluteBackgroundComponent->SetScaleY(m_PolluteBarScale.y);

	m_PolluteFillComponent = CreateComponent<Crystal::TextureComponent>("Background");
	m_PolluteFillComponent->SetWorldPosition({1920.0f / 2.0f - polluteFillFinalWidth * (1.0f - 0.5f), 1080.0f * 0.15f, 1.0f});
	m_PolluteFillComponent->AddMaterial(polluteFillTextureMat);
	m_PolluteFillComponent->SetScaleX(m_PolluteBarScale.x * 1.0f / (m_FillWidth * m_PolluteBarScale.x) * polluteFillFinalWidth * 0.5f);
	m_PolluteFillComponent->SetScaleY(m_PolluteBarScale.y);

	m_PolluteBarText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_PolluteBarText->SetSize(15.0f);
	m_PolluteBarText->SetText(L"Space Polllution Level");
	m_PolluteBarText->SetTextColor(Crystal::Vector3::White);
	m_PolluteBarText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_PolluteBarText->SetWorldPosition({1920.0f / 2.0f - polluteFillFinalWidth, 1080.0f * 0.15f + 17.0f, 0.0f});


	auto resourceBackgroundMat = Crystal::CreateShared<Crystal::Material>();
	resourceBackgroundMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/t_universal_panel_1.tga");
	resourceBackgroundMat->TintColor = {0.0f, 0.0f, 0.0f};
	resourceBackgroundMat->OpacityMultiplier = 0.5f;
	resourceBackgroundMat->bUseAlbedoTextureAlpha = true;


	float resourceBackgroundWidth = resourceBackgroundMat->AlbedoTexture.lock()->GetWidth();
	float resourceBackgroundHeight = resourceBackgroundMat->AlbedoTexture.lock()->GetHeight();

	float backgroundScaledWidth = 100.0f;
	float backgroundScaledHeight = 20.0f;


	auto goldBackgroundComponent = CreateComponent<Crystal::TextureComponent>("Background");
	goldBackgroundComponent->SetWorldPosition({200.0f - backgroundScaledWidth / 2.0f, 1080.0f * 0.1f, 0.0f});
	goldBackgroundComponent->AddMaterial(resourceBackgroundMat);
	goldBackgroundComponent->SetScaleX(1.0f / resourceBackgroundWidth * backgroundScaledWidth);
	goldBackgroundComponent->SetScaleY(1.0f / resourceBackgroundHeight * backgroundScaledHeight);




	auto goldTextureMat = Crystal::CreateShared<Crystal::Material>();
	goldTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/gold_ber.tga");
	goldTextureMat->bUseAlbedoTextureAlpha = true;
	

	float goldTextureWidth = goldTextureMat->AlbedoTexture.lock()->GetWidth();
	float goldTextureHeight = goldTextureMat->AlbedoTexture.lock()->GetHeight();



	auto goldTextureComponent = CreateComponent<Crystal::TextureComponent>("Background");
	goldTextureComponent->SetWorldPosition({
		200.0f - backgroundScaledWidth - backgroundScaledWidth / 2.0f + (backgroundScaledHeight * 1.2f), 1080.0f * 0.1f, 0.0f
	});
	goldTextureComponent->AddMaterial(goldTextureMat);
	goldTextureComponent->SetScaleX(1.0f / goldTextureWidth * backgroundScaledHeight * 0.8f);
	goldTextureComponent->SetScaleY(1.0f / goldTextureHeight * backgroundScaledHeight * 0.8f);



	m_GoldTextComponent = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_GoldTextComponent->SetSize(backgroundScaledHeight * 0.9f);
	m_GoldTextComponent->SetText(L"0");
	m_GoldTextComponent->SetTextColor(Crystal::Vector3::White);
	m_GoldTextComponent->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_GoldTextComponent->SetWorldPosition({
		200.0f - backgroundScaledWidth - backgroundScaledWidth / 2.0f + (backgroundScaledHeight * 1.7f) * 2.0f, 1080.0f * 0.1f, 1.0f
	});

	m_CruiserStoppedText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_CruiserStoppedText->SetSize(40.0f);
	m_CruiserStoppedText->SetText(L"Cruiser Stopped!!");
	m_CruiserStoppedText->SetTextColor({ 1.0f, 0.0f, 0.0f });
	m_CruiserStoppedText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_CruiserStoppedText->SetHorizontalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_CruiserStoppedText->SetWorldPosition({ 1920.0f / 2.0f, 1080.0f / 2.0f + 150.0f, 1.0f });
	m_CruiserStoppedText->SetHiddenInGame(true);


	m_UnderAttackText = CreateComponent<Crystal::TextRenderingComponent>("TextRenderingComponent");
	m_UnderAttackText->SetSize(40.0f);
	m_UnderAttackText->SetText(L"Base Is Under Attack");
	m_UnderAttackText->SetTextColor({ 1.0f, 0.0f, 0.0f });
	m_UnderAttackText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_UnderAttackText->SetHorizontalTextAlignment(Crystal::TextAlignment::TA_Center);
	m_UnderAttackText->SetWorldPosition({ 1920.0f / 2.0f, 1080.0f / 2.0f + 150.0f, 1.0f });
	m_UnderAttackText->SetHiddenInGame(true);


	HideBossHud();
	HideCruiserHud();
	HideSpaceStationHud();
}

void MyHUD::Update(float deltaTime)
{
	Actor::Update(deltaTime);

	auto mat = m_HitmarkerTextureComponent->GetMaterial(0);
	mat->OpacityMultiplier -= deltaTime * 5.0f;
	mat->OpacityMultiplier = max(0, mat->OpacityMultiplier);


	auto level = GetLevel().lock();
	if (level)
	{
		auto& quests = level->GetQuests();
		if (!quests.empty())
		{
			m_QuestMainText->SetHiddenInGame(false);
		}
		else
		{
			m_QuestMainText->SetHiddenInGame(true);
			for (int i = 0; i < 5; i++)
			{
				m_QuestCompletedMarkerOutlines[i]->SetHiddenInGame(true);
				m_QuestCompletedMarkers[i]->SetHiddenInGame(true);
				m_QuestDisplayTexts[i]->SetHiddenInGame(true);
			}
		}


		for (int i = 0; i < quests.size(); i++)
		{
			if (quests[i]->GetIsCompleted())
			{
				m_QuestCompletedMarkers[i]->SetHiddenInGame(false);
			}
			else
			{
				m_QuestCompletedMarkers[i]->SetHiddenInGame(true);
			}

			m_QuestCompletedMarkerOutlines[i]->SetHiddenInGame(false);
			m_QuestDisplayTexts[i]->SetHiddenInGame(false);


			std::string text = quests[i]->GetQuestText();
			std::wstring wText;
			if (!text.empty())
			{
				wText.assign(text.begin(), text.end());
			}

			Quest::EQuestType type = quests[i]->GetQuestType();
			switch (type)
			{
			case Quest::EQuestType::QT_None: break;
			case Quest::EQuestType::QT_Location:
				{
					
				}
				break;
			case Quest::EQuestType::QT_Destroy:
				{
					wText += L" : " + std::to_wstring(quests[i]->GetQuestTargetTypeCurrentCount()) + L" / " + std::to_wstring(
						quests[i]->GetQuestTargetTypeCount());
					
				}
				break;
			default: ;
			}

			m_QuestDisplayTexts[i]->SetText(wText);
		}

	}

	if(level)
	{
		auto playerPawn = Crystal::Cast<MyPlayerPawn>(level->GetPlayerPawn());
		auto inventory = playerPawn->GetInventory().lock();
		if(inventory)
		{
			uint32_t powerItemCount = inventory->GetItemCount(EItemType::IT_PowerPotion);
			uint32_t healItemCount = inventory->GetItemCount(EItemType::IT_HealPotion);
			uint32_t shieldItemCount = inventory->GetItemCount(EItemType::IT_ShieldPotion);

			if(powerItemCount > 0)
			{
				m_PowerPotionIcon->SetHiddenInGame(false);
				m_PowerPotionCount->SetHiddenInGame(false);
				m_PowerPotionCount->SetText(std::to_wstring(powerItemCount));
			}
			else
			{
				m_PowerPotionCount->SetHiddenInGame(true);
				m_PowerPotionIcon->SetHiddenInGame(true);
			}

			if (healItemCount > 0)
			{
				m_HealPotionIcon->SetHiddenInGame(false);
				m_HealPotionCount->SetHiddenInGame(false);
				m_HealPotionCount->SetText(std::to_wstring(healItemCount));
			}
			else
			{
				m_HealPotionCount->SetHiddenInGame(true);
				m_HealPotionIcon->SetHiddenInGame(true);
			}


			if (shieldItemCount > 0)
			{
				m_ShieldPotionIcon->SetHiddenInGame(false);
				m_ShieldPotionCount->SetHiddenInGame(false);
				m_ShieldPotionCount->SetText(std::to_wstring(shieldItemCount));
			}
			else
			{
				m_ShieldPotionCount->SetHiddenInGame(true);
				m_ShieldPotionIcon->SetHiddenInGame(true);
			}
		}
	}

	
}

void MyHUD::OnPlayerHealthUpdated(float currentHealth, float maxHealth) const
{
	float percent = currentHealth / maxHealth;
	percent = std::clamp(percent, 0.0f, 1.0f);


	auto playerFillFinalWidth = (m_BarBackgroundWidth * m_PlayerBarScale.x) - (2.0f * m_BarPadding * m_PlayerBarScale.y);

	m_PlayerFillComponent->SetWorldPosition({300.0f - playerFillFinalWidth * (1.0f - percent), 1080.0f * 0.95, 0.0f});
	m_PlayerFillComponent->SetScaleX(m_PlayerBarScale.x * 1 / (m_FillWidth * m_PlayerBarScale.x) * playerFillFinalWidth * percent);
}

void MyHUD::OnKrakenHealthUpdated(float currentHealth, float maxHealth) const
{
	float healthPercent = currentHealth / maxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

	auto krakenFillFinalWidth = (m_BarBackgroundWidth * m_KrakenBarScale.x) - (2.0f * m_BarPadding * m_KrakenBarScale.y);

	m_KrakenFillComponent->SetWorldPosition({1920.0f / 2.0f - krakenFillFinalWidth * (1.0f - healthPercent), 1080.0f * 0.20f, 0.0f});
	m_KrakenFillComponent->SetScaleX(m_KrakenBarScale.x * 1.0f / (m_FillWidth * m_KrakenBarScale.x) * krakenFillFinalWidth * healthPercent);
}

void MyHUD::OnCruiserHealthUpdated(float currentHealth, float maxHealth) const
{
	float healthPercent = currentHealth / maxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

	auto cruiserFinalFill = (m_BarBackgroundWidth * m_CruiserBarScale.x) - (2.0f * m_BarPadding * m_CruiserBarScale.y);

	m_CruiserFillComponent->SetWorldPosition({ 1920.0f / 2.0f - cruiserFinalFill * (1.0f - healthPercent), 1080.0f * 0.20f, 0.0f });
	m_CruiserFillComponent->SetScaleX(m_CruiserBarScale.x * 1.0f / (m_FillWidth * m_CruiserBarScale.x) * cruiserFinalFill * healthPercent);
}

void MyHUD::OnSpaceStationHealthUpdated(float currentHealth, float maxHealth) const
{
	float healthPercent = currentHealth / maxHealth;
	healthPercent = std::clamp(healthPercent, 0.0f, 1.0f);

	auto spaceStationFinalFill = (m_BarBackgroundWidth * m_SpaceStationBarScale.x) - (2.0f * m_BarPadding * m_SpaceStationBarScale.y);

	m_SpaceStationFillComponent->SetWorldPosition({ 1920.0f / 2.0f - spaceStationFinalFill * (1.0f - healthPercent), 1080.0f * 0.20f, 0.0f });
	m_SpaceStationFillComponent->SetScaleX(m_SpaceStationBarScale.x * 1.0f / (m_FillWidth * m_SpaceStationBarScale.x) * spaceStationFinalFill * healthPercent);
}

void MyHUD::OnPolluteGaugeUpdated(float currentPollute, float maxPolluteGauge) const
{
	float pollutePercent = currentPollute / maxPolluteGauge;
	pollutePercent = std::clamp(pollutePercent, 0.0f, 1.0f);

	auto polluteFillFinalWidth = (m_BarBackgroundWidth * m_PolluteBarScale.x) - (2.0f * m_BarPadding * m_PolluteBarScale.y);
	m_PolluteFillComponent->SetWorldPosition({1920.0f / 2.0f - polluteFillFinalWidth * (1.0f - pollutePercent), 1080.0f * 0.15f, 0.0f});
	m_PolluteFillComponent->SetScaleX(m_PolluteBarScale.x * 1.0f / (m_FillWidth * m_PolluteBarScale.x) * polluteFillFinalWidth * pollutePercent);
}


void MyHUD::OnPlayerHitSucceed()
{
	auto mat = m_HitmarkerTextureComponent->GetMaterial(0);
	mat->OpacityMultiplier = 1.0f;
}

void MyHUD::OnGoldUpdated(uint32_t goldAmount)
{
	m_GoldTextComponent->SetText(std::to_wstring(goldAmount));
}

void MyHUD::OnActorHovered(Crystal::Weak<Actor> hovered, float distFromPlayer)
{
	auto hoveredActor = hovered.lock();
	if (hoveredActor)
	{
		auto level = GetLevel().lock();
		auto pc = Crystal::Cast<Crystal::PlayerController>(level->GetPlayerController(0));
		auto actorPosition = hoveredActor->GetPosition();
		auto hoveredTexturePosition = pc->ProjectWorldToCameraSpace(actorPosition);

		m_HoveredTexture->SetWorldPosition({hoveredTexturePosition.x, hoveredTexturePosition.y, 3.0f});

		float scaleMultiplier = max(1.0f - (1.0f / 50000.0f * distFromPlayer), 0.0f);
		float finalScale = m_HoveredTextureMaxScale * scaleMultiplier;

		m_HoveredTexture->SetUnitScale(finalScale);
		m_HoveredTexture->SetHiddenInGame(false);
		m_HoveredDetailBackgroundTexture->SetHiddenInGame(false);
		m_HoveredDetailNameText->SetHiddenInGame(false);
		m_HoveredDetailDistanceText->SetHiddenInGame(false);


		m_HoveredDetailBackgroundTexture->SetWorldPosition({
			hoveredTexturePosition.x + m_HoveredTextureWidth * finalScale + m_HoveredDetailedBackgroundTextureWidth, hoveredTexturePosition.y, 2.0f
		});
		m_HoveredDetailNameText->SetWorldPosition({
			hoveredTexturePosition.x + m_HoveredTextureWidth * finalScale + 7.0f, hoveredTexturePosition.y + 7.0f + 20.0f, 0.0f
		});
		m_HoveredDetailDistanceText->SetWorldPosition({
			hoveredTexturePosition.x + m_HoveredTextureWidth * finalScale + 7.0f, hoveredTexturePosition.y - 7.0f, 0.0f
		});


		auto mat = m_HoveredDetailBackgroundTexture->GetMaterial(0);

		auto staticType = hoveredActor->StaticType();
		if (staticType == "GreenOreAsteroid" || staticType == "BlueOreAsteroid" || staticType == "YellowOreAsteroid")
		{
			mat->AlbedoColor = {1.0f, 1.0f, 0.0f};
		}
		else if (staticType == "AllyDrone" || staticType == "Cruiser")
		{
			mat->AlbedoColor = {0.0f, 1.0f, 0.0f};
		}
		else if (staticType == "Stargate" || staticType == "SpaceStation")
		{
			mat->AlbedoColor = {0.0f, 0.0f, 1.0f};
		}
		else // enenmy
		{
			mat->AlbedoColor = {1.0f, 0.0f, 0.0f};
		}


		std::wstring name(staticType.begin(), staticType.end());
		m_HoveredDetailNameText->SetText(name);

		auto distance = std::to_wstring(distFromPlayer / 10.0f);
		auto index = distance.find_first_of(L".");
		auto flooredDistance = distance.substr(0, index + 2);

		m_HoveredDetailDistanceText->SetText(flooredDistance + L"m");
	}
}

void MyHUD::OnActorNotHovered()
{
	m_HoveredTexture->SetHiddenInGame(true);
	m_HoveredDetailBackgroundTexture->SetHiddenInGame(true);

	m_HoveredDetailNameText->SetHiddenInGame(true);
	m_HoveredDetailDistanceText->SetHiddenInGame(true);
}

void MyHUD::ShowBossHud()
{
	m_KrakenBackgroundComponent->SetHiddenInGame(false);
	m_KrakenFillComponent->SetHiddenInGame(false);
	m_KrakenBarText->SetHiddenInGame(false);
	m_PolluteBackgroundComponent->SetHiddenInGame(false);
	m_PolluteFillComponent->SetHiddenInGame(false);
	m_PolluteBarText->SetHiddenInGame(false);
}

void MyHUD::HideBossHud()
{
	m_KrakenBackgroundComponent->SetHiddenInGame(true);
	m_KrakenFillComponent->SetHiddenInGame(true);
	m_KrakenBarText->SetHiddenInGame(true);
	m_PolluteBackgroundComponent->SetHiddenInGame(true);
	m_PolluteFillComponent->SetHiddenInGame(true);
	m_PolluteBarText->SetHiddenInGame(true);
}

void MyHUD::ShowCruiserHud()
{
	m_CruiserBackgroundComponent->SetHiddenInGame(false);
	m_CruiserFillComponent->SetHiddenInGame(false);
	m_CruiserBarText->SetHiddenInGame(false);
}

void MyHUD::HideCruiserHud()
{
	m_CruiserBackgroundComponent->SetHiddenInGame(true);
	m_CruiserFillComponent->SetHiddenInGame(true);
	m_CruiserBarText->SetHiddenInGame(true);
}

void MyHUD::ShowSpaceStationHud()
{
	m_SpaceStationBackgroundComponent->SetHiddenInGame(false);
	m_SpaceStationFillComponent->SetHiddenInGame(false);
	m_SpaceStationBarText->SetHiddenInGame(false);
	m_UnderAttackText->SetHiddenInGame(false);
}

void MyHUD::HideSpaceStationHud()
{

	m_SpaceStationBackgroundComponent->SetHiddenInGame(true);
	m_SpaceStationFillComponent->SetHiddenInGame(true);
	m_SpaceStationBarText->SetHiddenInGame(true);
	m_UnderAttackText->SetHiddenInGame(true);
}

void MyHUD::OnCruiserMoving()
{
	m_CruiserStoppedText->SetHiddenInGame(true);
}

void MyHUD::OnCruiserStopped()
{
	m_CruiserStoppedText->SetHiddenInGame(false);
}
