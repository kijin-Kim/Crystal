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

		UpdateTransfromHierarchy();
	}

	void TransformComponent::SetAttachment(TransformComponent* parentComponent)
	{
		/*서로가 서로의 Parent*/
		CS_FATAL(parentComponent->GetParentComponent() != this, "Component : %s 와 Component : %s는 서로가 서로의 부모입니다.",
			GetObjectName().c_str(), parentComponent->GetObjectName().c_str());
		CS_FATAL(GetOwner()->GetMainComponent(), "MainComponent가 존재 하지 않습니다. 먼저 MainComponent를 지정해주세요.",
			GetObjectName().c_str());
		CS_FATAL(GetOwner()->GetMainComponent() != this, "Component : %s 는 MainComponent입니다 Maincomponent는 다른 Component에 Attach 될 수 없습니다.",
			GetObjectName().c_str());

		Actor* owner = GetOwner();
		SetParentComponent(parentComponent);
		m_ParentComponent->AddChildComponent(parentComponent);

		/*현재 Component를 Transform Component Hierarchy로 이동시킵니다.*/
		owner->MoveToTransformComponentHierarchy(this);
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