#pragma once
#include "Pawn.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"

namespace Crystal {
	class CameraPawn : public Pawn
	{
	public:
		CameraPawn()
		{
			CameraComponent* cameraComponent = new CameraComponent();
			cameraComponent->SetWorldPosition(DirectX::XMFLOAT3(0, 100.0f, -500.0f));
			cameraComponent->SetFieldOfView(90.0f);
			cameraComponent->SetNearPlane(0.1f);
			cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f });
			cameraComponent->SetFarPlane(1000.0f);
			m_MainComponent = cameraComponent;

			ApplicationUtility::GetPlayerController()->SetMainCamera(cameraComponent);
		}
		virtual ~CameraPawn() = default;

		virtual void Start() override
		{
			Pawn::Start();
		}

		virtual void End() override
		{
			Pawn::End();
		}

		virtual void Update(float deltaTime) override
		{
			Pawn::Update(deltaTime);
		}

		virtual void SetupInputComponent(InputComponent* inputComponent) override
		{
			Pawn::SetupInputComponent(inputComponent);
			inputComponent->BindAxis("MoveForward", CS_AXIS_FN(Pawn::MoveForward));
			inputComponent->BindAction("Jump", EKeyStatus::KS_Repeat, CS_ACTION_FN(CameraPawn::Jump));
		}

		void MoveForward(float value)
		{
			__debugbreak();
		}

		void Jump()
		{
			__debugbreak();
		}
	private:
	};
}
