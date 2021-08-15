#include "cspch.h"
#include "SpringArmComponent.h"

BOOST_CLASS_EXPORT(Crystal::SpringArmComponent)

namespace Crystal {

	void SpringArmComponent::Update(const float deltaTime)
	{
		TransformComponent::Update(deltaTime);

		if (m_ParentComponent.expired())
		{
			CS_WARN("Spring Arm Component에 부모가 존재하지 않습니다");
			return;
		}


		m_LocalTransform = Matrix4x4::Scale(m_Scale);
		m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform, Matrix4x4::RotationQuaternion(m_RotationQuat));
		m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform,
			Matrix4x4::Translation(Vector3::Add(GetLocalPosition(), m_OffsetPosition)));

		// Multiply parent to get world
		m_WorldTransform = Matrix4x4::Multiply(m_LocalTransform, m_ParentComponent.lock()->GetWorldTransform());
		DirectX::XMFLOAT3 worldTargetPosition = { m_WorldTransform._41, m_WorldTransform._42, m_WorldTransform._43 };

		//=================================================================================================
		// 후크의 법칙 (Position)
		auto force = Vector3::Subtract(m_WorldLastPosition, worldTargetPosition);

		float magnitude = Vector3::Length(force);
		magnitude = (m_RestLength - magnitude);
		magnitude *= m_SpirngConstant;

		force = Vector3::Normalize(force);
		force = Vector3::Multiply(force, magnitude);
		auto accel = Vector3::Divide(force, 1.0f); // assume camera is 1kg


		// Drag
		float damping = sqrt(m_SpirngConstant) * 2.0f;
		accel = Vector3::Subtract(accel, Vector3::Multiply(m_Velocity, damping));

		auto newPosition = Vector3::Add(m_WorldLastPosition, Vector3::Multiply(m_Velocity, deltaTime));
		auto newVelocity = Vector3::Add(m_Velocity, Vector3::Multiply(accel, deltaTime));
		m_Velocity = newVelocity;
		//=================================================================================================
		SetWorldPosition(newPosition);
		m_WorldLastPosition = newPosition;
		
	}

	void SpringArmComponent::SetSpringConstant(const float springConstant)
	{
		m_SpirngConstant = springConstant;
	}

	void SpringArmComponent::SetRestLength(const float restLength)
	{
		m_RestLength = restLength;
	}

	void SpringArmComponent::SetOffsetPosition(const DirectX::XMFLOAT3& offset)
	{
		m_OffsetPosition = offset;
	}

}



