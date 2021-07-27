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
			
		}
		
		STATIC_TYPE_IMPLE(ProjectileMovementComponent)
	};
	
}
