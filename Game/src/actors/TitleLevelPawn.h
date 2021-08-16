#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/World/World.h"

class TitleLevelPawn : public Crystal::Pawn
{
public:
	TitleLevelPawn() = default;
	~TitleLevelPawn() override = default;

	void Initialize() override
	{
		auto cameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		cameraComponent->SetFieldOfView(85.0f);
		cameraComponent->SetNearPlane(20.0f);
		cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		cameraComponent->SetFarPlane(100000.0f);


		auto titleImage = Crystal::CreateShared<Crystal::Material>();
		titleImage->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/titleImage.png");
		titleImage->bUseAlbedoTextureAlpha = true;

		auto titleImageComponent = CreateComponent<Crystal::TextureComponent>("TitleImageComponent");
		titleImageComponent->SetUnitScale(0.5f);
		titleImageComponent->AddMaterial(titleImage);
		titleImageComponent->SetHiddenInGame(false);

		m_MainComponent = cameraComponent;
	}



	void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
		Pawn::SetupInputComponent(inputComponent);

		inputComponent->BindAction("OpenNewLevel", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(TitleLevelPawn::OpenGameLevel));
	}
	
	void OpenGameLevel()
	{
		auto world = Crystal::Cast<Crystal::World>(GetWorld());
		world->OpenLevel("GameLevel");
	}

	STATIC_TYPE_IMPLE(TitleLevelPawn)
	

};
