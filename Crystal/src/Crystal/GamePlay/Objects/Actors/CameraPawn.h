#pragma once
#include "Pawn.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/GamePlay/Components/MovementComponent.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/Math/Math.h"
#include "Crystal/Core/ApplicationUtility.h"
#include "Crystal/GamePlay/World/Level.h"

namespace Crystal {
	class CameraPawn : public Pawn
	{
	public:
		CameraPawn() = default;
		virtual ~CameraPawn() = default;

		void Initialize() override;

		void SetupInputComponent(InputComponent* inputComponent) override;
		
		void RotateYaw(float value);
		void RotatePitch(float value);
		void MoveForward(float value);
		void MoveRight(float value);
		void MoveUp(float value);
		void RollRight(float value);
		void BeginFire();


		STATIC_TYPE_IMPLE(CameraPawn)
	private:
		std::shared_ptr<MovementComponent> m_MovementComponent = nullptr;
	};
}
