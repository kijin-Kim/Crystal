#include "cspch.h"
#include "TransformComponent.h"

namespace Crystal {

	void TransformComponent::Update(const float deltaTime)
	{
		Component::Update(deltaTime);


		auto quaternion = Vector4::QuternionRotationAxis(Vector3::UnitX,
			DirectX::XMConvertToRadians(m_Pitch));
		quaternion = Vector4::QuaternionMultiply(quaternion,
			Vector4::QuternionRotationAxis(Vector3::UnitY, DirectX::XMConvertToRadians(m_Yaw)));
		quaternion = Vector4::QuaternionMultiply(quaternion,
			Vector4::QuternionRotationAxis(Vector3::UnitZ, DirectX::XMConvertToRadians(m_Roll)));


		m_Right = Vector3::Normalize(Vector3::RotateQuaternion(Vector3::UnitX, quaternion));
		m_Up = Vector3::Normalize(Vector3::RotateQuaternion(Vector3::UnitY, quaternion));
		m_Forward = Vector3::Normalize(Vector3::RotateQuaternion(Vector3::UnitZ, quaternion));


		const auto position = GetLocalPosition();
		const auto scale = Matrix4x4::Scale({ m_Scale, m_Scale, m_Scale });
		const DirectX::XMFLOAT4X4 rotation = Matrix4x4::RotationQuaternion(quaternion);
		const auto translation = Matrix4x4::Translation(position);

		m_LocalTransform = Matrix4x4::Multiply(scale, rotation);
		m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform, translation);


		/*Parent부터 자식 순으로 계산하여야 함*/
		if (m_ParentComponent)
			m_WorldTransform = Matrix4x4::Multiply(m_LocalTransform, m_ParentComponent->GetWorldTransform());
		else
			m_WorldTransform = m_LocalTransform;
	}
}