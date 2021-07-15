#pragma once
#include "MovementComponent.h"

namespace Crystal {

	class ProjectileMovementComponent : public MovementComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<MovementComponent>(*this);
		}
		
	public:
		ProjectileMovementComponent() = default;
		~ProjectileMovementComponent() override = default;

		void Begin() override
		{
			MovementComponent::Begin();
			
			auto xVel = Vector3::Multiply(m_TargetComponent->GetWorldRightVector(), m_Velocity.x);
			auto yVel = Vector3::Multiply(m_TargetComponent->GetWorldUpVector(), m_Velocity.y);
			auto zVel = Vector3::Multiply(m_TargetComponent->GetWorldForwardVector(), m_Velocity.z);

			auto newVel = Vector3::Add(Vector3::Add(xVel, yVel), zVel);
			
			m_TargetComponent->SetVelocity(newVel);
		}
		
		STATIC_TYPE_IMPLE(ProjectileMovementComponent)
	};
	
}
