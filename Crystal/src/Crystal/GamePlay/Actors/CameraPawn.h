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
			CS_LOG("-------");
		}

		virtual void SetupInputComponent(InputComponent* inputComponent) override
		{
			Pawn::SetupInputComponent(inputComponent);
			inputComponent->BindAxis("MoveForward", CS_AXIS_FN(CameraPawn::MoveForward));
			inputComponent->BindAxis("MoveRight", CS_AXIS_FN(CameraPawn::MoveRight));
			inputComponent->BindAxis("LookUp", CS_AXIS_FN(CameraPawn::AddControllerPitchInput));
			inputComponent->BindAxis("Turn", CS_AXIS_FN(CameraPawn::AddControllerYawInput));
		}

		void AddControllerYawInput(float value)
		{
			auto mainCamera = ((CameraComponent*)m_MainComponent);
			auto position = mainCamera->GetWorldPosition();
			mainCamera->RotateRollPitchYaw({ 0.0f, 0.0f,value * 0.05f });
		}

		void AddControllerPitchInput(float value)
		{
			auto mainCamera = ((CameraComponent*)m_MainComponent);
			auto position = mainCamera->GetWorldPosition();
			mainCamera->RotateRollPitchYaw({ 0.0f, -value * 0.05f, 0.0f });
		}

		void MoveForward(float value)
		{
			auto mainCamera = ((CameraComponent*)m_MainComponent);
			auto position = mainCamera->GetWorldPosition();
			auto forward = mainCamera->GetForward();

			DirectX::XMFLOAT3 newPosition = Vector3::Add(position, Vector3::Multiply(forward, DirectX::XMFLOAT3(value * 10.0f, value * 10.0f, value * 10.0f)));
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

	};
}
