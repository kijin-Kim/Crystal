#include "cspch.h"
#include "TransformComponent.h"

namespace Crystal {

	void TransformComponent::Update(const float deltaTime)
	{
		Component::Update(deltaTime);

		const auto position = GetLocalPosition();
		const auto scale = Matrix4x4::Scale({ m_Scale, m_Scale, m_Scale });
		const DirectX::XMFLOAT4X4 rotation = Matrix4x4::RotationQuaternion(m_Rotation);
		const auto translation = Matrix4x4::Translation(position);

		m_LocalTransform = Matrix4x4::Multiply(scale, rotation);
		m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform, translation);


		/*Parent부터 자식 순으로 계산하여야 함*/
		if (m_ParentComponent)
			m_WorldTransform = Matrix4x4::Multiply(m_LocalTransform, m_ParentComponent->GetWorldTransform());
		else
			m_WorldTransform = m_LocalTransform;
	}

	void TransformComponent::RotateRoll(float angle)
	{
		angle = DirectX::XMConvertToRadians(angle);

		m_Forward = Vector3::Normalize(Vector3::RotateQuaternion(Vector3::UnitZ, m_Rotation));
		
		auto newQuat = Vector4::QuaternionRotationAxis(m_Forward, angle);

		m_Up = Vector3::Normalize(Vector3::RotateQuaternion(m_Up, newQuat));
		m_Right = Vector3::Normalize(Vector3::RotateQuaternion(m_Right, newQuat));

		m_Rotation = Vector4::QuaternionMultiply(m_Rotation, newQuat);
	}


	void TransformComponent::RotatePitch(float angle)
	{

		angle = DirectX::XMConvertToRadians(angle);

		m_Right = Vector3::Normalize(Vector3::RotateQuaternion(Vector3::UnitX, m_Rotation));
		
		auto newQuat = Vector4::QuaternionRotationAxis(m_Right, angle);

		m_Up = Vector3::Normalize(Vector3::RotateQuaternion(m_Up, newQuat));
		m_Forward = Vector3::Normalize(Vector3::RotateQuaternion(m_Forward, newQuat));

		m_Rotation = Vector4::QuaternionMultiply(m_Rotation, newQuat);
	}


	void TransformComponent::RotateYaw(float angle)
	{
		angle = DirectX::XMConvertToRadians(angle);

		m_Up = Vector3::Normalize(Vector3::RotateQuaternion(Vector3::UnitY, m_Rotation));
		
		auto newQuat = Vector4::QuaternionRotationAxis(m_Up, angle);

		m_Right = Vector3::Normalize(Vector3::RotateQuaternion(m_Right, newQuat));
		m_Forward = Vector3::Normalize(Vector3::RotateQuaternion(m_Forward, newQuat));

		m_Rotation = Vector4::QuaternionMultiply(m_Rotation, newQuat);
	}

}