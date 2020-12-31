#pragma once
#include "Pawn.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/Math/Math.h"

namespace Crystal {
	class CameraPawn : public Pawn
	{
	public:
		CameraPawn()
		{
			CameraComponent* cameraComponent = new CameraComponent();
			cameraComponent->SetWorldPosition(DirectX::XMFLOAT3(0, 100.0f, -500.0f));
			cameraComponent->SetFieldOfView(60.0f);
			cameraComponent->SetNearPlane(0.1f);
			cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f });
			cameraComponent->SetFarPlane(10000.0f);
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
			inputComponent->BindAxis("MoveForward", CS_AXIS_FN(CameraPawn::MoveForward));
			inputComponent->BindAxis("MoveRight", CS_AXIS_FN(CameraPawn::MoveRight));
			inputComponent->BindAxis("LookUp", CS_AXIS_FN(CameraPawn::AddControllerPitchInput));
			inputComponent->BindAxis("Turn", CS_AXIS_FN(CameraPawn::AddControllerYawInput));
			inputComponent->BindAction("Jump", EKeyEvent::KE_Pressed, CS_ACTION_FN(CameraPawn::Jump));
			inputComponent->BindAction("Zoom", EKeyEvent::KE_Released, CS_ACTION_FN(CameraPawn::Zoom));
			inputComponent->BindAxis("Zoom2", CS_AXIS_FN(CameraPawn::Zoom2));
		}

		void AddControllerYawInput(float value)
		{
			auto mainCamera = ((CameraComponent*)m_MainComponent);
			auto position = mainCamera->GetWorldPosition();
			mainCamera->RotateYaw(value * 0.05f);
		}

		void AddControllerPitchInput(float value)
		{
			auto mainCamera = ((CameraComponent*)m_MainComponent);
			auto position = mainCamera->GetWorldPosition();
			mainCamera->RotatePitch(-value * 0.05f);
		}

		void MoveForward(float value)
		{
			auto mainCamera = ((CameraComponent*)m_MainComponent);
			auto position = mainCamera->GetWorldPosition();
			auto lookTo = mainCamera->GetLookTo();

			DirectX::XMFLOAT3 newPosition = Vector3::Add(position, Vector3::Multiply(lookTo, DirectX::XMFLOAT3(value * 10.0f, value * 10.0f, value * 10.0f)));
			mainCamera->SetWorldPosition(newPosition);
		}

		void MoveRight(float value)
		{
			auto mainCamera = ((CameraComponent*)m_MainComponent);
			auto position = mainCamera->GetWorldPosition();
			auto right = mainCamera->GetRight();

			DirectX::XMFLOAT3 newPosition = Vector3::Add(position, Vector3::Multiply(right, DirectX::XMFLOAT3(value * 10.0f, value * 10.0f, value * 10.0f)));
			mainCamera->SetWorldPosition(newPosition);
		}
		void Zoom2(float value)
		{
			__debugbreak();
		}
		void Zoom()
		{
			__debugbreak();
		}

		void Jump()
		{
		}

	};
}
