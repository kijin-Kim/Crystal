#pragma once
#include "TransformComponent.h"

namespace Crystal {

	class SpringArmComponent final : public TransformComponent
	{
	public:
		SpringArmComponent(const std::string& name) : TransformComponent(name)
		{

		}

		~SpringArmComponent() = default;

		void Update(const float deltaTime) override 
		{
			TransformComponent::Update(deltaTime);

		}

		void SetArmLength(const float armLength) { m_ArmLength = armLength; }
		float GetArmLength() const { return m_ArmLength; }

	private:
		float m_ArmLength = 0.0f;
	};

}
