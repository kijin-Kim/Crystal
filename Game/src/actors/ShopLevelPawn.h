#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

class ShopLevelPawn : public Crystal::Pawn
{
public:
	ShopLevelPawn() = default;
	~ShopLevelPawn() override = default;

	void Initialize() override
	{
		Pawn::Initialize();

		auto cameraComponent = CreateComponent<Crystal::CameraComponent>("CameraComponent");
		cameraComponent->SetFieldOfView(85.0f);
		cameraComponent->SetNearPlane(20.0f);
		cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
		cameraComponent->SetFarPlane(100000.0f);

		m_MainComponent = cameraComponent;
	}

};


