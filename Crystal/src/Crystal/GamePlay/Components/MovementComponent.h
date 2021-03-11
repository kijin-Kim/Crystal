#pragma once
#include "BaseComponents.h"

namespace Crystal {

	class MovementComponent : public Component
	{
	public:
		MovementComponent(const std::string& name) : Component(name) {}
		virtual ~MovementComponent() = default;

		void SetTargetComponent(TransformComponent* targetComponent) { m_TargetComponent = targetComponent; }



		void AddMovementInput(const DirectX::XMFLOAT3& force) 
		{ 
			float mass = m_TargetComponent->GetMass();
			DirectX::XMFLOAT3 acceleartion = Vector3::Divide(force, { mass, mass, mass });

			m_AccumulatedAcceleration = Vector3::Add(m_AccumulatedAcceleration, acceleartion); 
		}

		virtual void Update(float deltaTime)
		{
			Component::Update(deltaTime);

			// ������ġ = ��ġ + �ӵ� * dt +  0.5 * ���ӵ� * dt^2 (���ӵ��� ���� ��ġ ��ȭ�� ����)
			// �����ӵ� = �ӵ� * ���ΰ�� + ���ӵ� * dt (���ΰ�� = 0.99^dt)
			DirectX::XMFLOAT3 velocity = m_TargetComponent->GetVelocity();
			DirectX::XMFLOAT3 position = Vector3::Add(m_TargetComponent->GetPosition(), Vector3::Multiply(velocity, { deltaTime, deltaTime, deltaTime }));
			m_TargetComponent->SetPosition(position);

			float damping = pow(0.99f, deltaTime);
			velocity = Vector3::Add(Vector3::Multiply(velocity, { damping, damping, damping }), 
				Vector3::Multiply(m_AccumulatedAcceleration, { deltaTime, deltaTime, deltaTime }));
			m_TargetComponent->SetVelocity(velocity);

			m_AccumulatedAcceleration = { 0.0f, 0.0f, 0.0f };
		}

	private:
		DirectX::XMFLOAT3 m_AccumulatedAcceleration = { 0.0f, 0.0f, 0.0f };
		TransformComponent* m_TargetComponent = nullptr;
	};

}
