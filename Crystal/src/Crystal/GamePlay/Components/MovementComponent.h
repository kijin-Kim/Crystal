#pragma once
#include "Component.h"
#include "TransformComponent.h"

namespace Crystal {
	class Pawn;
}

namespace Crystal {
	class MovementComponent : public Component
	{
		SERIALIZE_PROPERTIES
		{
			ar & *m_TargetComponent;
			ar & m_Velocity;
		}

	public:
		MovementComponent() = default;
		~MovementComponent() override = default;

		void SetTargetVelocity(const DirectX::XMFLOAT3& velocity)
		{
			m_Velocity = velocity;
		}

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
		DirectX::XMFLOAT3 m_Velocity = Vector3::Zero;
	};


	class PawnMovementComponent : public MovementComponent
	{
		SERIALIZE_PROPERTIES
		{
			ar & *m_TargetComponent;
			ar& m_Velocity;
		}

	public:
		PawnMovementComponent() = default;
		~PawnMovementComponent() override = default;

		void Update(const float deltaTime) override
		{
			MovementComponent::Update(deltaTime);

			

		}

		STATIC_TYPE_IMPLE(PawnMovementComponent)
	};


}
