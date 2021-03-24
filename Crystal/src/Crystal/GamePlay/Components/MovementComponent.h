#pragma once
#include "BaseComponents.h"

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
			// ������ġ = ��ġ + �ӵ� * dt +  0.5 * ���ӵ� * dt^2 (���ӵ��� ���� ��ġ ��ȭ�� ����)
			// �����ӵ� = �ӵ� * ���ΰ�� + ���ӵ� * dt (���ΰ�� = 0.99^dt)
			const auto position = m_TargetComponent->GetPosition();
			const auto velocity = m_TargetComponent->GetVelocity();
			const auto mass = m_TargetComponent->GetMass();
			const auto acceleration = Vector3::Divide(m_ForceAccum, { mass, mass, mass });


			const auto newPosition = Vector3::Add(position, Vector3::Multiply(velocity, 
				{ deltaTime, deltaTime, deltaTime }));

			const float damping = 0.3f;
			const float dampingExponentDt = pow(damping, deltaTime);
			
			const auto newVelocity = Vector3::Add(Vector3::Multiply(velocity, 
				{ dampingExponentDt, dampingExponentDt, dampingExponentDt }),
				Vector3::Multiply(acceleration, { deltaTime, deltaTime, deltaTime }));

			m_TargetComponent->SetPosition(newPosition);
			m_TargetComponent->SetVelocity(newVelocity);
			
			m_ForceAccum = { 0.0f, 0.0f, 0.0f };
		}

	private:
		DirectX::XMFLOAT3 m_ForceAccum = { 0.0f, 0.0f, 0.0f };
		TransformComponent* m_TargetComponent = nullptr;
	};

}
