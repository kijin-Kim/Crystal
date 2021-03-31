#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/InputComponent.h"
#include "Crystal/GamePlay/Components/MovementComponent.h"
#include "Crystal/Resources/ShaderManager.h"

namespace Crystal {
	// User Controll-able actor
	class Pawn : public Actor
	{
	public:
		Pawn()
		{
		}

		virtual ~Pawn()
		{
		}

		void Begin() override
		{
			Actor::Begin();
		}
		void Update(const float deltaTime) override
		{
			Actor::Update(deltaTime); // Updating All Component
		}

		virtual void SetupInputComponent(InputComponent* inputComponent)
		{
		}

		STATIC_TYPE_IMPLE(Pawn)
	};
}
