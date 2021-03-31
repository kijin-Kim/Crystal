#pragma once
#include "Component.h"

namespace Crystal {
	class MovementComponent final : public Component
	{
	public:
		explicit MovementComponent(const std::string& name) : Component(name) {}
		~MovementComponent() override = default;

		void SetTargetComponent(TransformComponent* targetComponent) { m_TargetComponent = targetComponent; }

		void AddForce(const DirectX::XMFLOAT3& force)
		{
			m_ForceAccum = Vector3::Add(m_ForceAccum, force);
		}

		void Update(const float deltaTime) override
		{
			Component::Update(deltaTime);

			if (!m_TargetComponent->HasFiniteMass())
				return;

			auto position = m_TargetComponent->GetLocalPosition();
			auto velocity = m_TargetComponent->GetVelocity();

			position = Vector3::Add(position, Vector3::Multiply(velocity, deltaTime));
			m_TargetComponent->SetLocalPosition(position);

			float inverseMass = m_TargetComponent->GetInverseMass();
			auto accelertion = Vector3::Multiply(m_ForceAccum, inverseMass);

			velocity = Vector3::Add(velocity, Vector3::Multiply(accelertion, deltaTime));

			// Drag
			float damping = 0.3f;
			velocity = Vector3::Multiply(velocity, pow(damping, deltaTime));
			m_TargetComponent->SetVelocity(velocity);

			m_ForceAccum = Vector3::Zero;
		}

	private:
		DirectX::XMFLOAT3 m_ForceAccum = Vector3::Zero;
		TransformComponent* m_TargetComponent = nullptr;
	};
}
