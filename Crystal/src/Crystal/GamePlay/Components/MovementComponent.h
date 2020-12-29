#pragma once
#include "TransformComponent.h"

namespace Crystal {

	class MovementComponent : public TransformComponent
	{
	public:
		MovementComponent()
		{
			
		}
		virtual ~MovementComponent()
		{
		}

		virtual void Update(float deltaTime) override
		{
			Component::Update(deltaTime);
		}

	private:
		
		
	};
}

