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
		Pawn::Initialize();

		auto cameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		cameraComponent->SetProjectionMode(Crystal::ECameraProjectionMode::CPM_Orthographic);
		cameraComponent->SetNearPlane(20.0f);
		cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		cameraComponent->SetFarPlane(100000.0f);

		m_MainComponent = cameraComponent;
	}



	void SetupInputComponent(Crystal::InputComponent* inputComponent) override
	{
		Pawn::SetupInputComponent(inputComponent);

		inputComponent->BindAction("OpenGameLevel", Crystal::EKeyEvent::KE_Pressed, CS_ACTION_FN(TitleLevelPawn::OpenGameLevel));
	}
	
	void OpenGameLevel()
	{
		auto world = Crystal::Cast<Crystal::World>(GetWorld());
		world->SetCurrentLevelByName("GameLevel");
	}
	

};
