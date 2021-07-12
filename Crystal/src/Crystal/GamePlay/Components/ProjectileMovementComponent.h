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

		void Update(const float deltaTime) override
		{
			MovementComponent::Update(deltaTime);

			
			
		}


		
		STATIC_TYPE_IMPLE(ProjectileMovementComponent)
	};
	
}
