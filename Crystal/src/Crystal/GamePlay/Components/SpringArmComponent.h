#pragma once
#include "TransformComponent.h"
#include <corecrt_math.h>

namespace Crystal {
	class SpringArmComponent final : public TransformComponent
	{
	public:
		SpringArmComponent()
		{
		}
		~SpringArmComponent() override = default;

		void Update(const float deltaTime) override
		{
			if (!m_ParentComponent)
			{
				CS_WARN("Spring Arm Component에 부모가 존재하지 않습니다");
				return;
			}

			m_LocalTransform = Matrix4x4::Scale({ m_Scale, m_Scale, m_Scale });
			m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform, Matrix4x4::RotationQuaternion(m_Rotation));
			m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform,
				Matrix4x4::Translation(Vector3::Add(GetLocalPosition(), m_OffsetPosition)));

			// Multiply parent to get world
			m_WorldTransform = Matrix4x4::Multiply(m_LocalTransform, m_ParentComponent->GetWorldTransform());
			DirectX::XMFLOAT3 worldTargetPosition = { m_WorldTransform._41, m_WorldTransform._42, m_WorldTransform._43 };

			//=================================================================================================
			// 후크의 법칙 (Position) TODO : 회전
			auto force = Vector3::Multiply(Vector3::Subtract(m_WorldLastPosition, worldTargetPosition), -m_SpirngConstant);
			float damping = sqrt(m_SpirngConstant) * 2.0f;

			auto accel = Vector3::Divide(force, 1.0f);
			accel = Vector3::Subtract(accel, Vector3::Multiply(m_Velocity, damping));

			auto newPosition = Vector3::Add(m_WorldLastPosition, Vector3::Multiply(m_Velocity, deltaTime));
			auto newVelocity = Vector3::Add(m_Velocity, Vector3::Multiply(accel, deltaTime));
			m_Velocity = newVelocity;
			//=================================================================================================

			SetWorldPosition(newPosition);
			m_WorldLastPosition = newPosition;
		}

		void SetSpringConstant(const float springConstant) { m_SpirngConstant = springConstant; }
		void SetOffsetPosition(const DirectX::XMFLOAT3& offset) { m_OffsetPosition = offset; }

		STATIC_TYPE_IMPLE(SpringArmComponent)

	private:
		float m_SpirngConstant = 50.0f;
		DirectX::XMFLOAT3 m_OffsetPosition = Vector3::Zero;
		DirectX::XMFLOAT3 m_WorldLastPosition = Vector3::Zero;
	};
}
