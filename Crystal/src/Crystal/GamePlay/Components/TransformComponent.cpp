#include "cspch.h"
#include "TransformComponent.h"

BOOST_CLASS_EXPORT(Crystal::TransformComponent)

namespace Crystal {
	void TransformComponent::Update(float deltaTime)
	{
		Component::Update(deltaTime);

		const auto position = GetLocalPosition();
		const auto scale = Matrix4x4::Scale({m_Scale, m_Scale, m_Scale});
		const DirectX::XMFLOAT4X4 rotation = Matrix4x4::RotationQuaternion(m_Rotation);
		const auto translation = Matrix4x4::Translation(position);

		m_LocalTransform = Matrix4x4::Multiply(scale, rotation);
		m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform, translation);

		UpdateTransformHierarchy();
	}


	void TransformComponent::UpdateTransformHierarchy()
	{
		/*Parent부터 자식 순으로 계산하여야 함*/
		if (!m_ParentComponent.expired())
			m_WorldTransform = Matrix4x4::Multiply(m_LocalTransform, m_ParentComponent.lock()->GetWorldTransform());
		else
			m_WorldTransform = m_LocalTransform;
	}

	void TransformComponent::AttachTo(const std::shared_ptr<TransformComponent> component)
	{
		SetParentComponent(component);
		auto actor = Cast<Actor>(GetOuter());
		CS_FATAL(actor, "Outer가 존재하지 않습니다.");
		actor->MoveToTransformComponentHierarchy(Cast<TransformComponent>(shared_from_this()));
	}


	void TransformComponent::SetLocalPosition(const DirectX::XMFLOAT3& position)
	{
		m_LocalTransform._41 = position.x;
		m_LocalTransform._42 = position.y;
		m_LocalTransform._43 = position.z;
	}


	void TransformComponent::SetWorldPosition(const DirectX::XMFLOAT3& position)
	{
		DirectX::XMFLOAT3 newLocalPosition = position;

		if (!m_ParentComponent.expired())
		{
			auto newWorldTransform = m_WorldTransform;
			newWorldTransform._41 = position.x;
			newWorldTransform._42 = position.y;
			newWorldTransform._43 = position.z;

			auto newLocalTransform = Matrix4x4::Multiply(newWorldTransform,
			                                             Matrix4x4::Inverse(
				                                             m_ParentComponent.lock()->GetWorldTransform()));

			newLocalPosition = {newLocalTransform._41, newLocalTransform._42, newLocalTransform._43};
		}

		SetLocalPosition(newLocalPosition);
	}


	void TransformComponent::SetScale(const float scale)
	{
		m_Scale = scale;
	}

	void TransformComponent::SetVelocity(const DirectX::XMFLOAT3& velocity)
	{
		m_Velocity = velocity;
	}


	void TransformComponent::SetMass(const float mass)
	{
		m_InverseMass = 1.0f / mass;
	}


	void TransformComponent::SetInverseMass(const float inverseMass)
	{
		m_InverseMass = inverseMass;
	}

	void TransformComponent::SetRotation(const DirectX::XMFLOAT4& rotation)
	{
		m_Rotation = rotation;
	}

	void TransformComponent::SetWorldTransform(const DirectX::XMFLOAT4X4& transform)
	{
		m_WorldTransform = transform;
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

	DirectX::XMFLOAT3 TransformComponent::GetLocalPosition() const
	{
		return {m_LocalTransform._41, m_LocalTransform._42, m_LocalTransform._43};
	}


	DirectX::XMFLOAT3 TransformComponent::GetWorldPosition() const
	{
		return {m_WorldTransform._41, m_WorldTransform._42, m_WorldTransform._43};
	}


	const DirectX::XMFLOAT3& TransformComponent::GetVelocity() const
	{
		return m_Velocity;
	}

	float TransformComponent::GetScale()
	{
		return m_Scale;
	}


	float TransformComponent::GetMass() const
	{
		CS_FATAL(m_InverseMass != 0, "질량이 무한대 입니다. 먼저 HasFiniteMass()로 검사되어야합니다");
		return 1.0f / m_InverseMass;
	}

	float TransformComponent::GetInverseMass() const
	{
		return m_InverseMass;
	}


	bool TransformComponent::HasFiniteMass() const
	{
		return m_InverseMass != 0;
	}


	const DirectX::XMFLOAT3& TransformComponent::GetLocalRightVector() const
	{
		return m_Right;
	}


	const DirectX::XMFLOAT3& TransformComponent::GetLocalUpVector() const
	{
		return m_Up;
	}

	const DirectX::XMFLOAT3& TransformComponent::GetLocalForwardVector() const
	{
		return m_Forward;
	}

	DirectX::XMFLOAT3 TransformComponent::GetWorldRightVector() const
	{
		return {m_WorldTransform._11, m_WorldTransform._12, m_WorldTransform._13};
	}

	DirectX::XMFLOAT3 TransformComponent::GetWorldUpVector() const
	{
		return {m_WorldTransform._21, m_WorldTransform._22, m_WorldTransform._23};
	}

	DirectX::XMFLOAT3 TransformComponent::GetWorldForwardVector() const
	{
		return {m_WorldTransform._31, m_WorldTransform._32, m_WorldTransform._33};
	}

	void TransformComponent::SetParentComponent(const std::weak_ptr<TransformComponent>& component)
	{
		m_ParentComponent = component;
	}

	std::weak_ptr<TransformComponent> TransformComponent::GetParentComponent() const
	{
		return m_ParentComponent;
	}


	const DirectX::XMFLOAT4X4& TransformComponent::GetWorldTransform() const
	{
		return m_WorldTransform;
	}

	const DirectX::XMFLOAT4X4& TransformComponent::GetLocalTransform() const
	{
		return m_LocalTransform;
	}

	const DirectX::XMFLOAT4& TransformComponent::GetRotation() const
	{
		return m_Rotation;
	}

	bool TransformComponent::CanBeRendered() const
	{
		return false;
	}

	bool TransformComponent::IsCollisionEnabled() const
	{
		return false;
	}

}
