#pragma once
#include "Component.h"

namespace Crystal {
	/*�������� ��ġ�� ��Ÿ���� Ư���� ������ �ִ� ������Ʈ���� ���̽� Ŭ����
MainComponent�� �� �� �ְ�, �ִ� �ϳ��� �θ� ������ �θ��� ����� Transform�� ����*/
	class TransformComponent : public Component
	{
	public:
		TransformComponent() = default;
		~TransformComponent() override = default;

		void Update(const float deltaTime) override;

		void UpdateTransfromHierarchy()
		{
			/*Parent���� �ڽ� ������ ����Ͽ��� ��*/
			if (m_ParentComponent)
				m_WorldTransform = Matrix4x4::Multiply(m_LocalTransform, m_ParentComponent->GetWorldTransform());
			else
				m_WorldTransform = m_LocalTransform;
		}


		void SetLocalPosition(const DirectX::XMFLOAT3& position) { m_LocalPosition = position; }

		void SetWorldPosition(const DirectX::XMFLOAT3& position)
		{
			m_WorldTransform._41 = position.x;
			m_WorldTransform._42 = position.y;
			m_WorldTransform._43 = position.z;
		}

		void SetScale(const float scale) { m_Scale = scale; }
		void SetVelocity(const DirectX::XMFLOAT3& velocity) { m_Velocity = velocity; }
		void SetMass(const float mass) { m_InverseMass = 1.0f / mass; }
		void SetInverseMass(const float inverseMass) { m_InverseMass = inverseMass; }

		void RotateRoll(float angle);
		void RotatePitch(float angle);
		void RotateYaw(float angle);

		DirectX::XMFLOAT3 GetLocalPosition() const { return m_LocalPosition; }
		DirectX::XMFLOAT3 GetWorldPosition() const { return { m_WorldTransform._41, m_WorldTransform._42, m_WorldTransform._43 }; }
		const DirectX::XMFLOAT3& GetVelocity() const { return m_Velocity; }
		float GetMass() const
		{
			CS_FATAL(m_InverseMass != 0, "������ ���Ѵ� �Դϴ�. ���� HasFiniteMass()�� �˻�Ǿ���մϴ�");
			return 1.0f / m_InverseMass;
		}
		float GetInverseMass() const { return m_InverseMass; }
		bool HasFiniteMass() const { return m_InverseMass != 0; }

		const DirectX::XMFLOAT3& GetLocalRightVector() const { return m_Right; }
		const DirectX::XMFLOAT3& GetLocalUpVector()  const { return m_Up; }
		const DirectX::XMFLOAT3& GetLocalForwardVector() const { return m_Forward; }
		DirectX::XMFLOAT3 GetWorldRightVector() const { return { m_WorldTransform._11, m_WorldTransform._12, m_WorldTransform._13 }; }
		DirectX::XMFLOAT3 GetWorldUpVector() const { return { m_WorldTransform._21, m_WorldTransform._22, m_WorldTransform._23 }; }
		DirectX::XMFLOAT3 GetWorldForwardVector() const { return { m_WorldTransform._31, m_WorldTransform._32, m_WorldTransform._33 }; }

		void SetParentComponent(const std::shared_ptr<TransformComponent> component) { m_ParentComponent = std::move(component); }
		TransformComponent* GetParentComponent() const { return m_ParentComponent.get(); }

		void SetWorldTransform(const DirectX::XMFLOAT4X4& worldTransform) { m_WorldTransform = worldTransform; }
		void SetLocalTransform(const DirectX::XMFLOAT4X4& localTransform) { m_LocalTransform = localTransform; }

		const DirectX::XMFLOAT4X4& GetWorldTransform() const { return m_WorldTransform; }
		const DirectX::XMFLOAT4X4& GetLocalTransform() const { return m_LocalTransform; }

		const DirectX::XMFLOAT4& GetRotation() const { return m_Rotation; }


		STATIC_TYPE_IMPLE(TransformComponent)
	protected:
		/*OwnerShip�� ������ ���� ����*/
		std::shared_ptr<TransformComponent> m_ParentComponent = nullptr;
		std::vector<TransformComponent*> m_ChildComponents;

		DirectX::XMFLOAT4X4 m_WorldTransform = Matrix4x4::Identity();
		DirectX::XMFLOAT4X4 m_LocalTransform = Matrix4x4::Identity();
		float m_Scale = 1.0f; // Unit Scale�� ���� ����� ( Unit �̿��� Scale�� ����� ��� ������ ����Ǵ� �޽����� �븻 ���� �������־�� �� )

		DirectX::XMFLOAT3 m_Velocity = Vector3::Zero;
		float m_InverseMass = 10.0f;

		DirectX::XMFLOAT4 m_Rotation = Vector4::Quaternion::Identity;

		DirectX::XMFLOAT3 m_Right = Vector3::UnitX;
		DirectX::XMFLOAT3 m_Up = Vector3::UnitY;
		DirectX::XMFLOAT3 m_Forward = Vector3::UnitZ;

		DirectX::XMFLOAT3 m_LocalPosition = Vector3::Zero;
	};
}
