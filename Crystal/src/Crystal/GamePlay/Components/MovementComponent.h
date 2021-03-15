#pragma once
#include "BaseComponents.h"

namespace Crystal {

	class MovementComponent final : public Component
	{
	public:
		explicit MovementComponent(const std::string& name) : Component(name) {}
		~MovementComponent() override = default;

		void SetTargetComponent(TransformComponent* targetComponent) { m_TargetComponent = targetComponent; }



		void AddMovementInput(const DirectX::XMFLOAT3& force) 
		{
			const auto mass = m_TargetComponent->GetMass();
			const auto acceleration = Vector3::Divide(force, { mass, mass, mass });

			m_AccumulatedAcceleration = Vector3::Add(m_AccumulatedAcceleration, acceleration); 
		}
		
		void Update(const float deltaTime) override
		{
			Component::Update(deltaTime);
			
			// 최종위치 = 위치 + 속도 * dt +  0.5 * 가속도 * dt^2 (가속도에 의한 위치 변화는 생략)
			// 최종속도 = 속도 * 댐핑계수 + 가속도 * dt (댐핑계수 = 0.99^dt)
			const auto velocity = m_TargetComponent->GetVelocity();
			const auto position = m_TargetComponent->GetPosition();


			const auto newPosition = Vector3::Add(position, Vector3::Multiply(velocity, { deltaTime, deltaTime, deltaTime }));

			const float damping = 0.2f;
			const float dampingExponentDt = pow(damping, deltaTime);
			
			const auto newVelocity = Vector3::Add(Vector3::Multiply(velocity, { dampingExponentDt, dampingExponentDt, dampingExponentDt }),
			                                      Vector3::Multiply(m_AccumulatedAcceleration, { deltaTime, deltaTime, deltaTime }));

			
			m_TargetComponent->SetPosition(newPosition);
			m_TargetComponent->SetVelocity(newVelocity);
			

			m_AccumulatedAcceleration = { 0.0f, 0.0f, 0.0f };
		}

	private:
		DirectX::XMFLOAT3 m_AccumulatedAcceleration = { 0.0f, 0.0f, 0.0f };
		TransformComponent* m_TargetComponent = nullptr;
	};

}
