#pragma once
#include "Pawn.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/GamePlay/Components/MovementComponent.h"
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
			cameraComponent->SetPosition(DirectX::XMFLOAT3(0, 0.0f, -8000.0f));
			cameraComponent->SetFieldOfView(60.0f);
			cameraComponent->SetNearPlane(1.0f);
			cameraComponent->SetViewport({ 0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f });
			cameraComponent->SetFarPlane(100000.0f);
			m_MainComponent = cameraComponent;
			m_MainComponent->SetMass(60.0f);

			m_MovementComponent = CreateComponent<MovementComponent>("MovementComponent");
			m_MovementComponent->SetTargetComponent(m_MainComponent);



			ApplicationUtility::GetPlayerController().SetMainCamera(cameraComponent);
		}
		virtual ~CameraPawn() = default;

		void Begin() override
		{
			Pawn::Begin();
		}

		void End() override
		{
			Pawn::End();
		}

		void Update(const float deltaTime) override
		{
			Pawn::Update(deltaTime);
		}

		void SetupInputComponent(InputComponent* inputComponent) override
		{
			Pawn::SetupInputComponent(inputComponent);
			inputComponent->BindAxis("MoveForward", CS_AXIS_FN(CameraPawn::MoveForward));
			inputComponent->BindAxis("MoveRight", CS_AXIS_FN(CameraPawn::MoveRight));
			inputComponent->BindAxis("LookUp", CS_AXIS_FN(CameraPawn::RotatePitch));
			inputComponent->BindAxis("Turn", CS_AXIS_FN(CameraPawn::RotateYaw));


			inputComponent->BindAction("Fire", EKeyEvent::KE_Pressed, CS_ACTION_FN(CameraPawn::BeginFire));
		}

		void RotateYaw(float value)
		{
			//ApplicationUtility::GetPlayerController().ProcessYawInput(DirectX::XMConvertToRadians(value));
			const float valueScale = 0.1f;
			value *= valueScale;
			m_MainComponent->RotateYaw(DirectX::XMConvertToRadians(value));
		}

		void RotatePitch(float value)
		{
			//ApplicationUtility::GetPlayerController().ProcessPitchInput(DirectX::XMConvertToRadians(value));
			const float valueScale = 0.1f;
			value *= valueScale;
			m_MainComponent->RotatePitch(DirectX::XMConvertToRadians(-value));
		}

		void MoveForward(float value)
		{
			value *= 100000.0f;
			DirectX::XMFLOAT3 force = Vector3::Multiply(m_MainComponent->GetForward(), { value, value, value });
			m_MovementComponent->AddForce(force);
		}

		void MoveRight(float value)
		{
			value *= 100000.0f;
			DirectX::XMFLOAT3 force = Vector3::Multiply(m_MainComponent->GetRight(), { value, value, value });
			m_MovementComponent->AddForce(force);
		}

		void BeginFire()
		{
			CS_DEBUG_INFO("BeginFire!!");
			auto start = m_MainComponent->GetPosition();
			DirectX::XMFLOAT3 maxDistance = { 10000.0f, 10000.0f, 10000.0f };
			auto end = Vector3::Add(start, Vector3::Multiply(m_MainComponent->GetForward(), maxDistance));
			GetWorld()->DrawDebugLine(start, end);
		}

	};
}
