#pragma once
#include "Crystal/GamePlay/Components/ButtonComponent.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/ResourceManager.h"

class ClearLevelWidget : public Crystal::Actor
{
public:
	ClearLevelWidget() = default;
	~ClearLevelWidget() override = default;

	void Initialize() override
	{
		Actor::Initialize();

		m_Reward.Gold = Crystal::RandomIntInRange(1000, 2000);
		m_Reward.ItemType.push_back(EItemType::IT_HealPotion);
		m_Reward.ItemType.push_back(EItemType::IT_PowerPotion);
		m_Reward.ItemType.push_back(EItemType::IT_ShieldPotion);
		m_Reward.ItemType.push_back(EItemType::IT_Ore);
		m_Reward.ItemCount.push_back(Crystal::RandomIntInRange(0, 10));
		m_Reward.ItemCount.push_back(Crystal::RandomIntInRange(0, 10));
		m_Reward.ItemCount.push_back(Crystal::RandomIntInRange(0, 10));
		m_Reward.ItemCount.push_back(Crystal::RandomIntInRange(0, 10));


		auto& resourceManager = Crystal::ResourceManager::Instance();

		auto backgroundMat = Crystal::CreateShared<Crystal::Material>();
		backgroundMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/UIBackground.jpg");
		backgroundMat->bUseAlbedoTextureAlpha = true;


		float backgroundWidth = (float)backgroundMat->AlbedoTexture.lock()->GetWidth();
		float backgroundHeight = (float)backgroundMat->AlbedoTexture.lock()->GetHeight();

		auto background = CreateComponent<Crystal::TextureComponent>("Background");
		background->AddMaterial(backgroundMat);
		background->SetWorldPosition({1920.0f / 2.0f, 1080.0f / 2.0f, 0.0f});
		background->SetScaleX(1.0f / backgroundWidth * 1920.0f / 2.0f);
		background->SetScaleY(1.0f / backgroundHeight * 1080.0f / 2.0f);


		auto backgroundMat2 = Crystal::CreateShared<Crystal::Material>();
		backgroundMat2->AlbedoColor = {48.0f / 255.0f, 48.0f / 255.0f, 48.0f / 255.0f};
		backgroundMat2->OpacityTexture = resourceManager.GetTexture("assets/textures/WhiteRect.png");

		float backgroundWidth1 = (float)backgroundMat2->OpacityTexture.lock()->GetWidth();
		float backgroundHeight1 = (float)backgroundMat2->OpacityTexture.lock()->GetHeight();


		auto background1 = CreateComponent<Crystal::TextureComponent>("Background1");
		background1->AddMaterial(backgroundMat2);
		background1->SetWorldPosition({1920.0f / 2.0f, 1080.0f / 2.0f, 1.0f});
		background1->SetScaleX(1.0f / backgroundWidth1 * 1920.0f / 2.0f * 0.8f);
		background1->SetScaleY(1.0f / backgroundHeight1 * 1080.0f / 2.0f * 0.9f);


		auto missionClearedText = CreateComponent<Crystal::TextRenderingComponent>("RewardText");
		missionClearedText->SetWorldPosition({1920.0f / 2.0f, 1080.0f / 2.0f + 150.0f, 1.0f});
		missionClearedText->SetSize(50.0f);
		missionClearedText->SetTextColor({1.0f, 1.0f, 1.0f});
		missionClearedText->SetText(L"Mission Cleared!!");
		missionClearedText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
		missionClearedText->SetHorizontalTextAlignment(Crystal::TextAlignment::TA_Center);


		auto rewardBackgroundMat = Crystal::CreateShared<Crystal::Material>();
		rewardBackgroundMat->AlbedoColor = {59.0f / 255.0f, 59.0f / 255.0f, 59.0f / 255.0f};
		rewardBackgroundMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

		float width = (float)rewardBackgroundMat->OpacityTexture.lock()->GetWidth();
		float height = (float)rewardBackgroundMat->OpacityTexture.lock()->GetHeight();


		std::vector<Crystal::Shared<Crystal::Material>> thumbNails;
		thumbNails.push_back(Crystal::CreateShared<Crystal::Material>());
		thumbNails.push_back(Crystal::CreateShared<Crystal::Material>());
		thumbNails.push_back(Crystal::CreateShared<Crystal::Material>());
		thumbNails.push_back(Crystal::CreateShared<Crystal::Material>());


		thumbNails[0]->AlbedoTexture = resourceManager.GetTexture("assets/textures/electric_potion.tga");
		thumbNails[0]->bUseAlbedoTextureAlpha = true;
		thumbNails[1]->AlbedoTexture = resourceManager.GetTexture("assets/textures/potion_heal.tga");
		thumbNails[1]->bUseAlbedoTextureAlpha = true;
		thumbNails[2]->AlbedoTexture = resourceManager.GetTexture("assets/textures/shield_potion.tga");
		thumbNails[2]->bUseAlbedoTextureAlpha = true;
		thumbNails[3]->AlbedoTexture = resourceManager.GetTexture("assets/textures/diamond1.tga");
		thumbNails[3]->bUseAlbedoTextureAlpha = true;


		for (int i = 0; i < 2; i++)
		{
			auto backgroundTexture = CreateComponent<Crystal::TextureComponent>("ButtonComponent");
			backgroundTexture->AddMaterial(rewardBackgroundMat);
			backgroundTexture->SetWorldPosition({1920.0f / 2.0f - width / 2.0f - 10.0f / 2.0f - 10.0f * i - width * i, 1080.0f / 2.0f - 100.0f, 2.0f});
			backgroundTexture->SetUnitScale(0.5f);


			auto itemThumbNail = CreateComponent<Crystal::TextureComponent>("ThumbNail");
			itemThumbNail->SetWorldPosition({1920.0f / 2.0f - width / 2.0f - 10.0f / 2.0f - 10.0f * i - width * i, 1080.0f / 2.0f - 100.0f, 2.0f});
			itemThumbNail->AddMaterial(thumbNails[i]);
			itemThumbNail->SetUnitScale(1 / 512.0f * 120.0f / 2.0f);

			if (m_Reward.ItemCount[i] == 0)
			{
				itemThumbNail->SetHiddenInGame(true);
			}


			auto countText = CreateComponent<Crystal::TextRenderingComponent>("Text");
			countText->SetText(std::to_wstring(m_Reward.ItemCount[i]));
			countText->SetSize(20.0f);
			countText->SetWorldPosition({
				1920.0f / 2.0f - width / 2.0f - 10.0f / 2.0f - 10.0f * i - width * i - 147.0f / 2.0f + 3.0f, 1080.0f / 2.0f - 100.0f + 147.0f / 2.0f - 3.0f, 0.0f
			});

			if(m_Reward.ItemCount[i] == 0)
			{
				countText->SetHiddenInGame(true);
			}
		}


		for (int i = 0; i < 2; i++)
		{
			auto backgroundTexture = CreateComponent<Crystal::TextureComponent>("ButtonComponent");
			backgroundTexture->AddMaterial(rewardBackgroundMat);
			backgroundTexture->SetWorldPosition({1920.0f / 2.0f + width / 2.0f + 10.0f / 2.0f + 10.0f * i + width * i, 1080.0f / 2.0f - 100.0f, 2.0f});
			backgroundTexture->SetUnitScale(0.5f);

			auto itemThumbNail = CreateComponent<Crystal::TextureComponent>("ThumbNail");
			itemThumbNail->SetWorldPosition({1920.0f / 2.0f + width / 2.0f + 10.0f / 2.0f + 10.0f * i + width * i, 1080.0f / 2.0f - 100.0f, 2.0f});
			itemThumbNail->AddMaterial(thumbNails[i + 2]);
			itemThumbNail->SetUnitScale(1 / 512.0f * 120.0f / 2.0f);

			if (m_Reward.ItemCount[i] == 0)
			{
				itemThumbNail->SetHiddenInGame(true);
			}


			auto countText = CreateComponent<Crystal::TextRenderingComponent>("Text");
			countText->SetText(std::to_wstring(m_Reward.ItemCount[i + 2]));
			countText->SetSize(20.0f);
			countText->SetWorldPosition({
				1920.0f / 2.0f + width / 2.0f + 10.0f / 2.0f + 10.0f * i + width * i - 147.0f / 2.0f + 3.0f, 1080.0f / 2.0f - 100.0f + 147.0f / 2.0f - 3.0f, 0.0f
			});

			if (m_Reward.ItemCount[i] == 0)
			{
				countText->SetHiddenInGame(true);
			}
		}


		auto goldBackground = CreateComponent<Crystal::TextureComponent>("ButtonComponent");
		goldBackground->AddMaterial(backgroundMat);
		goldBackground->SetWorldPosition({1920.0f / 2.0f, 1080.0f / 2.0f - 100.0f - 147.0f - 20.0f, 2.0f});
		goldBackground->SetScaleX(1.0f / backgroundWidth * 174.0f / 2.0f * 4.0f);
		goldBackground->SetScaleY(1.0f / backgroundHeight * 1080.0f / 2.0f * 0.1f);


		auto goldTextureMat = Crystal::CreateShared<Crystal::Material>();
		goldTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/gold_ber.tga");
		goldTextureMat->bUseAlbedoTextureAlpha = true;

		auto goldTexture = CreateComponent<Crystal::TextureComponent>("Background");
		goldTexture->AddMaterial(goldTextureMat);
		goldTexture->SetWorldPosition({
			1920.0f / 2.0f - 174.0f / 2.0f * 4.0f + 1080.0f / 2.0f * 0.1f * 0.8f + 20.0f, 1080.0f / 2.0f - 100.0f - 147.0f - 20.0f, 3.0f
		});
		goldTexture->SetScaleX(1.0f / 512.0f * 1080.0f / 2.0f * 0.1f * 0.8f);
		goldTexture->SetScaleY(1.0f / 512.0f * 1080.0f / 2.0f * 0.1f * 0.8f);

		auto goldText = CreateComponent<Crystal::TextRenderingComponent>("GoldText");
		goldText->SetSize(50.0f);
		goldText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
		goldText->SetWorldPosition({1920.0f / 2.0f - 174.0f / 2.0f * 4.0f + 1080.0f * 0.1f * 0.8f + 30.0f, 1080.0f / 2.0f - 100.0f - 147.0f - 20.0f, 0.0f});
		goldText->SetText(std::to_wstring(m_Reward.Gold));


		auto exitTextureMat = Crystal::CreateShared<Crystal::Material>();
		exitTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/x-2.tga");
		exitTextureMat->bUseAlbedoTextureAlpha = true;


		auto exitTexture = CreateComponent<Crystal::TextureComponent>("TextureComponent");
		exitTexture->AddMaterial(exitTextureMat);
		exitTexture->SetWorldPosition({1920.0f - 130.0f / 2.0f - 15.0f, (1080.0f - 50.0f) - 130.0f / 2.0f, 2.0f});
		exitTexture->SetScaleX(1 / 512.0f * 80.0f / 2.0f);
		exitTexture->SetScaleY(1 / 512.0f * 80.0f / 2.0f);


		auto exitButtonNormalMat = Crystal::CreateShared<Crystal::Material>();
		exitButtonNormalMat->AlbedoColor = {48.0f / 255.0f, 48.0f / 255.0f, 48.0f / 255.0f};
		exitButtonNormalMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");


		auto exitButtonHoveredMat = Crystal::CreateShared<Crystal::Material>();
		exitButtonHoveredMat->AlbedoColor = {40.0f / 255.0f, 40.0f / 255.0f, 40.0f / 255.0f};
		exitButtonHoveredMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

		auto exitButtonPressedMat = Crystal::CreateShared<Crystal::Material>();
		exitButtonPressedMat->AlbedoColor = {31.0f / 255.0f, 31.0f / 255.0f, 31.0f / 255.0f};
		exitButtonPressedMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

		float exitWidth = (float)exitButtonNormalMat->OpacityTexture.lock()->GetWidth();
		float exitHeight = (float)exitButtonNormalMat->OpacityTexture.lock()->GetHeight();


		auto exitButton = CreateComponent<Crystal::ButtonComponent>("ButtonComponent");
		exitButton->SetWorldPosition({1920.0f - 130.0f / 2.0f - 15.0f, (1080.0f - 50.0f) - 130.0f / 2.0f, 1.0f});
		exitButton->SetNormalMaterial(exitButtonNormalMat);
		exitButton->SetHoveredMaterial(exitButtonHoveredMat);
		exitButton->SetPressedMaterial(exitButtonPressedMat);
		exitButton->SetScaleX(1.0f / exitWidth * 130.0f / 2.0f);
		exitButton->SetScaleY(1.0f / exitHeight * 130.0f / 2.0f);


		exitButton->BindOnButtonClickedEvent([this]()
		{
			auto world = GetWorld().lock();
			world->PopLevel();
		});
	}

	const QuestReward& GetReward() const { return m_Reward; }

private:
	QuestReward m_Reward;
};
