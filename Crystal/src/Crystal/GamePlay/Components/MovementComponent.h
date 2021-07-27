#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

namespace Crystal {
	class Pawn;
}

namespace Crystal {
	class MovementComponent : public Component
	{
		SERIALIZE_PROPERTIES
		{
			ar & *m_TargetComponent;
		}

	public:
		MovementComponent() = default;
		~MovementComponent() override = default;

		void SetTargetComponent(std::shared_ptr<TransformComponent> targetComponent)
		{
			m_TargetComponent = std::move(targetComponent);
		}

		Shared<Pawn> GetTargetComponentOwner() const;

		void Update(const float deltaTime) override
		{
			Component::Update(deltaTime);

			if (!m_TargetComponent->HasFiniteMass())
				return;			
		}

		STATIC_TYPE_IMPLE(MovementComponent)
	protected:
		std::shared_ptr<TransformComponent> m_TargetComponent = nullptr;
	};


	class PawnMovementComponent : public MovementComponent
	{
	public:
		PawnMovementComponent() = default;
		~PawnMovementComponent() override = default;

		void Update(const float deltaTime) override
		{
			MovementComponent::Update(deltaTime);

			auto owner = GetTargetComponentOwner();
			auto inputVector = owner->GetInputVector();

			if(Vector3::IsZero(inputVector))
			{
				return;
			}

			m_TargetComponent->AddForce(Vector3::Multiply(Vector3::Normalize(inputVector), m_MaxAcceleration));

			owner->ClearInputVector();

		}

		void SetMaxAcceleration(float accel)
		{
			m_MaxAcceleration = accel;
		}

		STATIC_TYPE_IMPLE(PawnMovementComponent)

	private:
		float m_MaxAcceleration = 0.0f;
	};


}
