#pragma once
#include "MovementComponent.h"

namespace Crystal {

	class ProjectileMovementComponent : public MovementComponent
	{

	public:
		ProjectileMovementComponent() = default;
		~ProjectileMovementComponent() override = default;

		void SetProjectileMaxAcceleration(float acceleration)
		{
			m_ProjectileMaxAcceleration = acceleration;
		}

		void Begin() override
		{
			MovementComponent::Begin();
			m_TargetComponent->AddForce(Vector3::Multiply(Vector3::Normalize(m_TargetComponent->GetWorldForwardVector()), m_ProjectileMaxAcceleration));
		}

		
		STATIC_TYPE_IMPLE(ProjectileMovementComponent)

		float m_ProjectileMaxAcceleration = 1000.0f;
	};
	
}


