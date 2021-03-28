#pragma once
#include "TransformComponent.h"
#include <corecrt_math.h>

namespace Crystal {

	class SpringArmComponent final : public TransformComponent
	{
	public:
		SpringArmComponent(const std::string& name) : TransformComponent(name)
		{

		}
		~SpringArmComponent() override = default;

		void Update(const float deltaTime) override
		{
			TransformComponent::Update(deltaTime);

			auto targetPosition = GetWorldPosition();

			auto force = Vector3::Multiply(Vector3::Subtract(m_LastWorldPosition, targetPosition), -m_SpirngConstant);
			float damping = sqrt(m_SpirngConstant) * 2.0f;
			
			auto accel = Vector3::Divide(force, 1.0f);
			accel = Vector3::Subtract(accel, Vector3::Multiply(m_Velocity, damping));

			auto newPosition = Vector3::Add(m_LastWorldPosition, Vector3::Multiply(m_Velocity, deltaTime));
			SetWorldPosition(newPosition);
			auto newVelocity = Vector3::Add(m_Velocity, Vector3::Multiply(accel, deltaTime));
			SetVelocity(newVelocity);

			m_LastWorldPosition = newPosition;
		}

		void SetSpringConstant(const float springConstant) { m_SpirngConstant = springConstant; }

	private:
		float m_SpirngConstant = 80.0f;
		DirectX::XMFLOAT3 m_LastWorldPosition = GetLocalPosition();
	};

}
