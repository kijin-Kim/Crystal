#pragma once
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Resources/ResourceManager.h"

class ShopWidget : public Crystal::Actor
{
public:
	ShopWidget() = default;
	~ShopWidget() override = default;

	void Initialize() override
	{
		Actor::Initialize();


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


		auto goldBackgroundMat = Crystal::CreateShared<Crystal::Material>();
		goldBackgroundMat->AlbedoColor = {48.0f / 255.0f, 48.0f / 255.0f, 48.0f / 255.0f};
		goldBackgroundMat->OpacityTexture = resourceManager.GetTexture("assets/textures/WhiteRect.png");

		float goldBackgroundWidth = (float)goldBackgroundMat->OpacityTexture.lock()->GetWidth();
		float goldBackgroundHeight = (float)goldBackgroundMat->OpacityTexture.lock()->GetHeight();

		auto goldBackground = CreateComponent<Crystal::TextureComponent>("Background");
		goldBackground->AddMaterial(goldBackgroundMat);
		goldBackground->SetWorldPosition({158.0f + 643.0f / 2.0f + 643.0f + 158.0f * 2.0f, 26.0f + 25.0f / 2.0f, 1.0f});
		goldBackground->SetScaleX(1.0f / goldBackgroundWidth * 643.0f / 2.0f);
		goldBackground->SetScaleY(1.0f / goldBackgroundHeight * 25.0f);


		auto goldTextureMat = Crystal::CreateShared<Crystal::Material>();
		goldTextureMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/gold_ber.tga");
		goldTextureMat->bUseAlbedoTextureAlpha = true;

		auto goldTexture = CreateComponent<Crystal::TextureComponent>("Background");
		goldTexture->AddMaterial(goldTextureMat);
		goldTexture->SetWorldPosition({158.0f + 25.0f + 5.0f + 643.0f + 158.0f * 2.0f, 26.0f + 25.0f / 2.0f, 1.0f});
		goldTexture->SetScaleX(1.0f / 512.0f * 25.0f);
		goldTexture->SetScaleY(1.0f / 512.0f * 25.0f);

		m_GoldText = CreateComponent<Crystal::TextRenderingComponent>("GoldText");
		m_GoldText->SetSize(20.0f);
		m_GoldText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
		m_GoldText->SetWorldPosition({158.0f + 25.0f * 2.0f + 5.0f + 8.0f + 643.0f + 158.0f * 2.0f, 26.0f + 25.0f / 2.0f, 0.0f});
		m_GoldText->SetText(L"0000");


		for (int i = 0; i < 2; i++)
		{
			auto backgroundMat2 = Crystal::CreateShared<Crystal::Material>();
			backgroundMat2->AlbedoColor = {48.0f / 255.0f, 48.0f / 255.0f, 48.0f / 255.0f};
			backgroundMat2->OpacityTexture = resourceManager.GetTexture("assets/textures/WhiteRect.png");

			float width = (float)backgroundMat2->OpacityTexture.lock()->GetWidth();
			float height = (float)backgroundMat2->OpacityTexture.lock()->GetHeight();

			auto text = CreateComponent<Crystal::TextRenderingComponent>("Text");
			text->SetSize(40.0f);
			if (i == 0)
			{
				text->SetText(L"Shop");
			}
			else
			{
				text->SetText(L"Inventory");
			}

			text->SetTextColor({1.0f, 1.0f, 1.0f});
			text->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
			text->SetWorldPosition({158.0f + 40.0f + 643.0f * (float)i + 158.0f * 2.0f * (float)i, (1080.0f - 50.0f) - 130.0f / 2.0f, 1.0f});


			auto background1 = CreateComponent<Crystal::TextureComponent>("Background1");
			background1->AddMaterial(backgroundMat2);
			background1->SetWorldPosition({158.0f + 643.0f / 2.0f + 643.0f * (float)i + 158.0f * 2.0f * (float)i, (1080.0f - 50.0f) - 130.0f / 2.0f, 1.0f});
			background1->SetScaleX(1.0f / width * 643.0f / 2.0f);
			background1->SetScaleY(1.0f / height * 130.0f / 2.0f);


			auto background2 = CreateComponent<Crystal::TextureComponent>("Background2");
			background2->AddMaterial(backgroundMat2);
			background2->SetWorldPosition({158.0f + 643.0f / 2.0f + 643.0f * (float)i + 158.0f * 2.0f * (float)i, (1080.0f - 200.0f) - 798.0f / 2.0f, 1.0f});
			background2->SetScaleX(1.0f / width * 643.0f / 2.0f);
			background2->SetScaleY(1.0f / height * 798.0f / 2.0f);
		}

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				auto testButtonNormalMat = Crystal::CreateShared<Crystal::Material>();
				testButtonNormalMat->AlbedoColor = {59.0f / 255.0f, 59.0f / 255.0f, 59.0f / 255.0f};
				testButtonNormalMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

				auto testButtonHoveredMat = Crystal::CreateShared<Crystal::Material>();
				testButtonHoveredMat->AlbedoColor = {40.0f / 255.0f, 40.0f / 255.0f, 40.0f / 255.0f};
				testButtonHoveredMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

				auto testButtonPressedMat = Crystal::CreateShared<Crystal::Material>();
				testButtonPressedMat->AlbedoColor = {31.0f / 255.0f, 31.0f / 255.0f, 31.0f / 255.0f};
				testButtonPressedMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

				float width = (float)testButtonNormalMat->OpacityTexture.lock()->GetWidth();
				float height = (float)testButtonNormalMat->OpacityTexture.lock()->GetHeight();


				auto countText = CreateComponent<Crystal::TextRenderingComponent>("Text");
				countText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
				countText->SetText(L"0000 Gold");
				countText->SetSize(16.0f);
				countText->SetWorldPosition({
					158.0f + 50.0f + (50 * (float)j) + (width * (float)j) + 643.0f * (float)0.0f + 158.0f * 2.0f * (float)0.0f + 2.0f,
					(1080.0f - 200.0f) - (42.0f * (float)(i + 1) + height * (float)i) - 16.0f - 2.0f, 0.0f
				});
				countText->SetHiddenInGame(true);
				m_ShopTexts.push_back(countText);


				auto itemThumbNail = CreateComponent<Crystal::TextureComponent>("ThumbNail");
				itemThumbNail->SetWorldPosition({
					158.0f + 50.0f + width / 2.0f + (50 * (float)j) + (width * (float)j) + 643.0f * (float)0.0f + 158.0f * 2.0f * (float)0.0f,
					(1080.0f - 200.0f) - (height / 2.0f + 42.0f * (float)(i + 1) + height * (float)i), 3.0f
				});
				itemThumbNail->AddMaterial(Crystal::CreateShared<Crystal::Material>());
				itemThumbNail->SetUnitScale(1 / 512.0f * 120.0f / 2.0f);
				m_ShopThumbNails.push_back(itemThumbNail);


				auto testButton = CreateComponent<Crystal::ButtonComponent>("ButtonComponent");
				testButton->SetWorldPosition({
					158.0f + 50.0f + width / 2.0f + (50 * (float)j) + (width * (float)j) + 643.0f * (float)0.0f + 158.0f * 2.0f * (float)0.0f,
					(1080.0f - 200.0f) - (height / 2.0f + 42.0f * (float)(i + 1) + height * (float)i), 2.0f
				});
				testButton->SetNormalMaterial(testButtonNormalMat);
				testButton->SetHoveredMaterial(testButtonHoveredMat);
				testButton->SetPressedMaterial(testButtonPressedMat);
				testButton->SetUnitScale(0.5f);


				m_ShopButtons.push_back(testButton);
			}
		}

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				auto testButtonNormalMat = Crystal::CreateShared<Crystal::Material>();
				testButtonNormalMat->AlbedoColor = {59.0f / 255.0f, 59.0f / 255.0f, 59.0f / 255.0f};
				testButtonNormalMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

				auto testButtonHoveredMat = Crystal::CreateShared<Crystal::Material>();
				testButtonHoveredMat->AlbedoColor = {40.0f / 255.0f, 40.0f / 255.0f, 40.0f / 255.0f};
				testButtonHoveredMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

				auto testButtonPressedMat = Crystal::CreateShared<Crystal::Material>();
				testButtonPressedMat->AlbedoColor = {31.0f / 255.0f, 31.0f / 255.0f, 31.0f / 255.0f};
				testButtonPressedMat->OpacityTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/WhiteRect.png");

				float width = (float)testButtonNormalMat->OpacityTexture.lock()->GetWidth();
				float height = (float)testButtonNormalMat->OpacityTexture.lock()->GetHeight();

				auto countText = CreateComponent<Crystal::TextRenderingComponent>("Text");
				countText->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
				countText->SetText(L"0000");
				countText->SetSize(16.0f);
				countText->SetWorldPosition({
					158.0f + 50.0f + (50 * (float)j) + (width * (float)j) + 643.0f * (float)1.0f + 158.0f * 2.0f * (float)1.0f + 2.0f,
					(1080.0f - 200.0f) - (42.0f * (float)(i + 1) + height * (float)i) - 16.0f - 2.0f, 0.0f
				});
				countText->SetHiddenInGame(true);
				m_InventoryTexts.push_back(countText);

				auto itemThumbNail = CreateComponent<Crystal::TextureComponent>("ThumbNail");
				itemThumbNail->SetWorldPosition({
					158.0f + 50.0f + width / 2.0f + (50 * (float)j) + (width * (float)j) + 643.0f * (float)1.0f + 158.0f * 2.0f * (float)1.0f,
					(1080.0f - 200.0f) - (height / 2.0f + 42.0f * (float)(i + 1) + height * (float)i), 3.0f
				});
				itemThumbNail->AddMaterial(Crystal::CreateShared<Crystal::Material>());
				itemThumbNail->SetUnitScale(1 / 512.0f * 120.0f / 2.0f);
				m_InventoryThumbNails.push_back(itemThumbNail);


				auto testButton = CreateComponent<Crystal::ButtonComponent>("ButtonComponent");
				testButton->SetWorldPosition({
					158.0f + 50.0f + width / 2.0f + (50 * (float)j) + (width * (float)j) + 643.0f * (float)1.0f + 158.0f * 2.0f * (float)1.0f,
					(1080.0f - 200.0f) - (height / 2.0f + 42.0f * (float)(i + 1) + height * (float)i), 2.0f
				});
				testButton->SetNormalMaterial(testButtonNormalMat);
				testButton->SetHoveredMaterial(testButtonHoveredMat);
				testButton->SetPressedMaterial(testButtonPressedMat);
				testButton->SetUnitScale(0.5f);

				m_InventoryButtons.push_back(testButton);
			}
		}

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

		float width = (float)exitButtonNormalMat->OpacityTexture.lock()->GetWidth();
		float height = (float)exitButtonNormalMat->OpacityTexture.lock()->GetHeight();


		auto exitButton = CreateComponent<Crystal::ButtonComponent>("ButtonComponent");
		exitButton->SetWorldPosition({1920.0f - 130.0f / 2.0f - 15.0f, (1080.0f - 50.0f) - 130.0f / 2.0f, 1.0f});
		exitButton->SetNormalMaterial(exitButtonNormalMat);
		exitButton->SetHoveredMaterial(exitButtonHoveredMat);
		exitButton->SetPressedMaterial(exitButtonPressedMat);
		exitButton->SetScaleX(1.0f / width * 130.0f / 2.0f);
		exitButton->SetScaleY(1.0f / height * 130.0f / 2.0f);


		exitButton->BindOnButtonClickedEvent([this]()
		{
			auto world = GetWorld().lock();
			world->PopLevel();
		});
	}


	void Update(float deltaTime) override
	{
		Actor::Update(deltaTime);

		auto level = GetLevel().lock();

		auto inventory = Crystal::Cast<Inventory>(level->GetActorByClass("Inventory"));
		if (inventory)
		{
			m_GoldText->SetText(std::to_wstring(inventory->GetGold()));
		}

		if (inventory)
		{
			std::array<EItemType, 3> itemTypes = {
				EItemType::IT_PowerPotion,
				EItemType::IT_HealPotion,
				EItemType::IT_ShieldPotion
			};
			{
				int index = 0;
				for (int i = 0; i < itemTypes.size(); i++)
				{
					auto type = itemTypes[i];
					auto mat = m_ShopThumbNails[i]->GetMaterials()[0];
					auto thumbNailMat = inventory->GetItemThumbNails(type);
					mat->AlbedoTexture = thumbNailMat->AlbedoTexture;
					mat->AlbedoColor = thumbNailMat->AlbedoColor;
					mat->TintColor = thumbNailMat->TintColor;
					mat->OpacityTexture = thumbNailMat->OpacityTexture;
					mat->bUseAlbedoTextureAlpha = thumbNailMat->bUseAlbedoTextureAlpha;
					mat->Opacity = thumbNailMat->Opacity;
					mat->OpacityMultiplier = thumbNailMat->OpacityMultiplier;


					auto itemCost = inventory->GetItemCost(type);
					m_ShopTexts[index]->SetText(std::to_wstring(itemCost));
					m_ShopTexts[index]->SetHiddenInGame(false);

					m_ShopButtons[index]->BindOnButtonClickedEvent([inventory, itemCost, type]()
					{
						auto gold = inventory->GetGold();
						if (gold < itemCost)
						{
							return;
						}

						inventory->SubtractGold(itemCost);
						inventory->AddItem(type);
					});

					index++;
				}

				for (int i = index; i < m_InventoryTexts.size(); i++)
				{
					m_ShopThumbNails[i]->SetHiddenInGame(true);
					m_ShopTexts[i]->SetHiddenInGame(true);
					m_ShopButtons[i]->BindOnButtonClickedEvent([]()
					{
					});
				}
			}


			{
				auto& items = inventory->GetItems();
				int index = 0;
				for (auto& pair : items)
				{
					auto type = pair.first;
					auto count = pair.second;

					auto mat = m_InventoryThumbNails[index]->GetMaterials()[0];
					auto thumbNailMat = inventory->GetItemThumbNails(type);
					mat->AlbedoTexture = thumbNailMat->AlbedoTexture;
					mat->AlbedoColor = thumbNailMat->AlbedoColor;
					mat->TintColor = thumbNailMat->TintColor;
					mat->OpacityTexture = thumbNailMat->OpacityTexture;
					mat->bUseAlbedoTextureAlpha = thumbNailMat->bUseAlbedoTextureAlpha;
					mat->Opacity = thumbNailMat->Opacity;
					mat->OpacityMultiplier = thumbNailMat->OpacityMultiplier;

					m_InventoryTexts[index]->SetText(std::to_wstring(count));
					m_InventoryTexts[index]->SetHiddenInGame(false);


					auto itemCost = inventory->GetItemCost(type);
					m_InventoryButtons[index]->BindOnButtonClickedEvent([inventory, itemCost, type]()
					{
						if (inventory->GetItemCount(type) <= 0)
						{
							return;
						}

						inventory->RemoveItem(type);
						inventory->AddGold(itemCost);
					});


					index++;
				}

				for (int i = index; i < m_InventoryTexts.size(); i++)
				{
					m_InventoryThumbNails[i]->SetHiddenInGame(true);
					m_InventoryTexts[i]->SetHiddenInGame(true);

					m_InventoryButtons[i]->BindOnButtonClickedEvent([]()
					{
					});
				}
			}
		}
	}

private:
	Crystal::Shared<Crystal::TextRenderingComponent> m_GoldText;
	std::vector<Crystal::Shared<Crystal::TextureComponent>> m_ShopThumbNails;
	std::vector<Crystal::Shared<Crystal::TextureComponent>> m_InventoryThumbNails;

	std::vector<Crystal::Shared<Crystal::TextRenderingComponent>> m_ShopTexts;
	std::vector<Crystal::Shared<Crystal::TextRenderingComponent>> m_InventoryTexts;

	std::vector<Crystal::Shared<Crystal::ButtonComponent>> m_ShopButtons;
	std::vector<Crystal::Shared<Crystal::ButtonComponent>> m_InventoryButtons;
};


