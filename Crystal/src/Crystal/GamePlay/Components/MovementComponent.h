#pragma once
#include "Component.h"
#include "TransformComponent.h"

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
}
