#pragma once
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/Resources/ResourceManager.h"

class GameOverLevelPawn : public Crystal::Pawn
{
public:
	GameOverLevelPawn() = default;
	~GameOverLevelPawn() override = default;

	void Initialize() override
	{
		auto cameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		cameraComponent->SetFieldOfView(85.0f);
		cameraComponent->SetNearPlane(20.0f);
		cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		cameraComponent->SetFarPlane(100000.0f);


		auto image = Crystal::CreateShared<Crystal::Material>();
		image->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/GameOver.png");
		image->bUseAlbedoTextureAlpha = true;

		auto imageComponent = CreateComponent<Crystal::TextureComponent>("ImageComponent");
		imageComponent->SetUnitScale(0.5f);
		imageComponent->AddMaterial(image);
		imageComponent->SetHiddenInGame(false);

		m_MainComponent = cameraComponent;
	}



	void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
		Pawn::SetupInputComponent(inputComponent);

		inputComponent->BindAction("OpenNewLevel", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(GameOverLevelPawn::OpenTitleLevel));
	}

	void OpenTitleLevel()
	{
		auto world = Crystal::Cast<Crystal::World>(GetWorld());
		world->OpenLevel("TitleLevel");
	}

	STATIC_TYPE_IMPLE(GameOverLevelPawn)
};

class GameClearLevelPawn : public Crystal::Pawn
{
public:
	GameClearLevelPawn() = default;
	~GameClearLevelPawn() override = default;

	void Initialize() override
	{
		auto cameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		cameraComponent->SetFieldOfView(85.0f);
		cameraComponent->SetNearPlane(20.0f);
		cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		cameraComponent->SetFarPlane(100000.0f);


		auto image = Crystal::CreateShared<Crystal::Material>();
		image->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/GameClear.png");
		image->bUseAlbedoTextureAlpha = true;

		auto imageComponent = CreateComponent<Crystal::TextureComponent>("ImageComponent");
		imageComponent->SetUnitScale(0.5f);
		imageComponent->AddMaterial(image);
		imageComponent->SetHiddenInGame(false);

		m_MainComponent = cameraComponent;
	}



	void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
		Pawn::SetupInputComponent(inputComponent);

		inputComponent->BindAction("OpenNewLevel", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(GameClearLevelPawn::OpenTitleLevel));
	}

	void OpenTitleLevel()
	{
		auto world = Crystal::Cast<Crystal::World>(GetWorld());
		world->OpenLevel("TitleLevel");
	}

	STATIC_TYPE_IMPLE(GameClearLevelPawn)
};
