#pragma once
#include "Pawn.h"

namespace Crystal {

	class CameraPawn  : public Pawn
	{
	public:
		CameraPawn() = default;
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
