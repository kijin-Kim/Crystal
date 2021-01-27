#pragma once
#include "Pawn.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/Math/Math.h"
#include "Crystal/Core/ApplicationUtility.h"

namespace Crystal {
	class CameraPawn : public Pawn
	{
	public:
		CameraPawn()
		{
			auto cameraComponent = CreateComponent<CameraComponent>("CameraComponent");
			cameraComponent->SetWorldPosition(DirectX::XMFLOAT3(0, 0.0f, -10.0f));
			cameraComponent->SetFieldOfView(60.0f);
			cameraComponent->SetNearPlane(0.1f);
			cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f });
			cameraComponent->SetFarPlane(10000.0f);
			m_MainComponent = cameraComponent;

			ApplicationUtility::GetPlayerController().SetMainCamera(cameraComponent);
		}
		virtual ~CameraPawn() = default;

		virtual void Begin() override
		{
			Pawn::Begin();
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
			inputComponent->BindAxis("MoveForward", CS_AXIS_FN(CameraPawn::MoveForward));
			inputComponent->BindAxis("MoveRight", CS_AXIS_FN(CameraPawn::MoveRight));
			inputComponent->BindAxis("LookUp", CS_AXIS_FN(CameraPawn::AddControllerPitchInput));
			inputComponent->BindAxis("Turn", CS_AXIS_FN(CameraPawn::AddControllerYawInput));

			inputComponent->BindAction("LockMouse", EKeyEvent::KE_Pressed, CS_ACTION_FN(CameraPawn::BeginMouseLock));
			inputComponent->BindAction("LockMouse", EKeyEvent::KE_Released, CS_ACTION_FN(CameraPawn::EndMouseLock));
		}

		void AddControllerYawInput(float value)
		{
			ApplicationUtility::GetPlayerController().ProcessYawInput(value);
		}

		void AddControllerPitchInput(float value)
		{
			ApplicationUtility::GetPlayerController().ProcessPitchInput(value);
		}

		void MoveForward(float value)
		{
			auto mainCamera = (CameraComponent*)m_MainComponent;
			auto position = mainCamera->GetWorldPosition();
			auto forward = mainCamera->GetForward();

			DirectX::XMFLOAT3 newPosition = Vector3::Add(position, Vector3::Multiply(forward, DirectX::XMFLOAT3(value, value, value)));
			mainCamera->SetWorldPosition(newPosition);
		}

		void MoveRight(float value)
		{
			auto mainCamera = (CameraComponent*)m_MainComponent;
			auto position = mainCamera->GetWorldPosition();
			auto right = mainCamera->GetRight();

			DirectX::XMFLOAT3 newPosition = Vector3::Add(position, Vector3::Multiply(right, DirectX::XMFLOAT3(value, value, value)));
			mainCamera->SetWorldPosition(newPosition);
		}

		void BeginMouseLock()
		{
			m_bMouseIsLocked = true;
			//ApplicationUtility::GetPlayerController()->LockMouseCursor();
		}

		void EndMouseLock()
		{
			m_bMouseIsLocked = false;
		}

	private:
		bool m_bMouseIsLocked = false;
	};
}
