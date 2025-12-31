#pragma once

#pragma once
#include "Inventory.h"
#include "Crystal/GamePlay/Components/ButtonComponent.h"
#include "Crystal/GamePlay/Components/TextRenderingComponent.h"
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/ResourceManager.h"

class StargateWidget : public Crystal::Actor
{
public:
	StargateWidget() = default;
	~StargateWidget() override = default;

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


		std::string levelNames[3] = {
			"MiningLevel",
			"EscortLevel",
			"KrakenLevel"
		};


		std::wstring displayNames[3] = {
			L"Mine Mission",
			L"Escort Mission",
			L"Boss Mission"
		};

		for (int i = 0; i < 3; i++)
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


			auto testButton = CreateComponent<Crystal::ButtonComponent>("ButtonComponent");
			testButton->SetWorldPosition({1920.0f / 2.0f, 1080.0f - 108.0f - 252.0f / 2.0f - (54.0f + 252.0f) * (i), 2.0f});
			testButton->SetNormalMaterial(testButtonNormalMat);
			testButton->SetHoveredMaterial(testButtonHoveredMat);
			testButton->SetPressedMaterial(testButtonPressedMat);
			testButton->SetScaleX(1.0f / width * 1920.0f / 2.0f * 0.7f);
			testButton->SetScaleY(1.0f / height * 252.0f / 2.0f);


			auto text = CreateComponent<Crystal::TextRenderingComponent>("ButtonComponent");
			text->SetHorizontalTextAlignment(Crystal::TextAlignment::TA_Center);
			text->SetVerticalTextAlignment(Crystal::TextAlignment::TA_Center);
			text->SetWorldPosition({ 1920.0f / 2.0f, 1080.0f - 108.0f - 252.0f / 2.0f - (54.0f + 252.0f) * (i), 0.0f });
			text->SetText(displayNames[i]);
			text->SetSize(50.0f);
			


			testButton->BindOnButtonClickedEvent([this, levelNames, i]()
			{
					auto world = GetWorld().lock();
					world->PopLevel();
					world->PushLevel(levelNames[i]);
			});
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
	}
};


