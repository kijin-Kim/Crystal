#include "cspch.h"
#include "CameraPawn.h"

namespace Crystal {

	void CameraPawn::Initialize()
	{
		Pawn::Initialize();

		auto cameraComponent = CreateComponent<CameraComponent>("CameraComponent");
		cameraComponent->SetLocalPosition(DirectX::XMFLOAT3(0, 0.0f, -15000.0f));
		cameraComponent->SetFieldOfView(60.0f);
		cameraComponent->SetNearPlane(1000.0f);
		cameraComponent->SetViewport({0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f});
		cameraComponent->SetFarPlane(10000000.0f);
		cameraComponent->SetMass(60.0f);

		m_MainComponent = cameraComponent;

		m_MovementComponent = CreateComponent<MovementComponent>("MovementComponent");
		m_MovementComponent->SetTargetComponent(m_MainComponent);
	}

	void CameraPawn::SetupInputComponent(InputComponent* inputComponent)
	{
		Pawn::SetupInputComponent(inputComponent);
		inputComponent->BindAxis("MoveForward", CS_AXIS_FN(CameraPawn::MoveForward));
		inputComponent->BindAxis("MoveRight", CS_AXIS_FN(CameraPawn::MoveRight));
		inputComponent->BindAxis("MoveUp", CS_AXIS_FN(CameraPawn::MoveUp));
		inputComponent->BindAxis("LookUp", CS_AXIS_FN(CameraPawn::RotatePitch));
		inputComponent->BindAxis("Turn", CS_AXIS_FN(CameraPawn::RotateYaw));

		inputComponent->BindAxis("RollRight", CS_AXIS_FN(CameraPawn::RollRight));

		inputComponent->BindAction("Fire", EKeyEvent::KE_Pressed, CS_ACTION_FN(CameraPawn::BeginFire));
	}

	void CameraPawn::RotateYaw(float value)
	{
		//ApplicationUtility::GetPlayerController().ProcessYawInput(DirectX::XMConvertToRadians(value));
		const float valueScale = 0.1f;
		value *= valueScale;
		m_MainComponent->RotateYaw(value);
	}

	void CameraPawn::RotatePitch(float value)
	{
		//ApplicationUtility::GetPlayerController().ProcessPitchInput(DirectX::XMConvertToRadians(value));
		const float valueScale = 0.1f;
		value *= valueScale;
		m_MainComponent->RotatePitch(-value);
	}

	void CameraPawn::MoveForward(float value)
	{
		value *= 100000.0f;
		DirectX::XMFLOAT3 force = Vector3::Multiply(m_MainComponent->GetLocalForwardVector(), {value, value, value});
		//m_MovementComponent->AddForce(force);
	}

	void CameraPawn::MoveRight(float value)
	{
		value *= 100000.0f;
		DirectX::XMFLOAT3 force = Vector3::Multiply(m_MainComponent->GetLocalRightVector(), {value, value, value});
		//m_MovementComponent->AddForce(force);
	}

	void CameraPawn::MoveUp(float value)
	{
		value *= 100000.0f;
		DirectX::XMFLOAT3 force =
			Crystal::Vector3::Multiply(m_MainComponent->GetLocalUpVector(), {value, value, value});
		//m_MovementComponent->AddForce(force);
	}

	void CameraPawn::RollRight(float value)
	{
		m_MainComponent->RotateRoll(-value);
	}

	void CameraPawn::BeginFire()
	{
		CS_DEBUG_INFO("BeginFire!!");
		auto start = m_MainComponent->GetWorldPosition();
		DirectX::XMFLOAT3 maxDistance = {10000.0f, 10000.0f, 10000.0f};
		auto end = Vector3::Add(start, Vector3::Multiply(m_MainComponent->GetLocalForwardVector(), maxDistance));

		auto level = Crystal::Cast<Level>(GetOuter());
		if (level)
			level->DrawDebugLine(start, end);
	}
}
