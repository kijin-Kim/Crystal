#pragma once
#include "Component.h"

namespace Crystal {

	/*�������� ��ġ�� ��Ÿ���� Ư���� ������ �ִ� ������Ʈ���� ���̽� Ŭ����
MainComponent�� �� �� �ְ�, �ִ� �ϳ��� �θ� ������ �θ��� ����� Transform�� ����*/
	class TransformComponent : public Component
	{
	public:
		TransformComponent(const std::string& name) : Component(name)
		{
		}

		~TransformComponent() override = default;

		void Update(const float deltaTime) override;

		/*Component�� Transform�� �ٸ� Ư�� Component�� ���ӵǵ��� �մϴ�.*/
		void SetAttachment(TransformComponent* parentComponent)
		{
			/*���ΰ� ������ Parent*/
			CS_FATAL(parentComponent->GetParent() != this, "Component : %s �� Component : %s�� ���ΰ� ������ �θ��Դϴ�.",
				GetName().c_str(), parentComponent->GetName().c_str());
			CS_FATAL(GetOwner()->GetMainComponent(), "MainComponent�� ���� ���� �ʽ��ϴ�. ���� MainComponent�� �������ּ���.",
				GetName().c_str());
			CS_FATAL(GetOwner()->GetMainComponent() != this, "Component : %s �� MainComponent�Դϴ� Maincomponent�� �ٸ� Component�� Attach �� �� �����ϴ�.",
				GetName().c_str());

			Actor* owner = GetOwner();
			m_Parent = parentComponent;

			/*���� Component�� Transform Component Hierarchy�� �̵���ŵ�ϴ�.*/
			owner->MoveToTransformComponentHierarchy(this);

		}

		void SetLocalPosition(const DirectX::XMFLOAT3& position)
		{
			m_LocalTransform._41 = position.x;
			m_LocalTransform._42 = position.y;
			m_LocalTransform._43 = position.z;
		}

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

		void SetRoll(const float roll) { m_Roll = roll; }
		void SetPitch(const float pitch) { m_Pitch = pitch; }
		void SetYaw(const float yaw) { m_Yaw = yaw; }

		float GetRoll() const { return m_Roll; }
		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }

		void AddRoll(const float roll) { m_Roll += roll; }
		void AddPitch(const float pitch) { m_Pitch += pitch; }
		void AddYaw(const float yaw) { m_Yaw += yaw; }

		DirectX::XMFLOAT3 GetLocalPosition() const { return { m_LocalTransform._41, m_LocalTransform._42, m_LocalTransform._43 }; }
		DirectX::XMFLOAT3 GetWorldPosition() const { return { m_WorldTransform._41, m_WorldTransform._42, m_WorldTransform._43 }; }
		const DirectX::XMFLOAT3& GetVelocity() const { return m_Velocity; }
		float GetMass() const
		{
			CS_FATAL(m_InverseMass != 0, "������ ���Ѵ� �Դϴ�. ���� HasFiniteMass()�� �˻�Ǿ���մϴ�");
			return 1.0f / m_InverseMass;
		}
		float GetInverseMass() const { return m_InverseMass; }
		bool HasFiniteMass() const { return m_InverseMass != 0; }

		const DirectX::XMFLOAT3& GetRight() const { return m_Right; }
		const DirectX::XMFLOAT3& GetUp()  const { return m_Up; }
		const DirectX::XMFLOAT3& GetForward() const { return m_Forward; }


		TransformComponent* GetParent() const { return m_Parent; }
		const DirectX::XMFLOAT4X4& GetWorldTransform() const { return m_WorldTransform; }
		const DirectX::XMFLOAT4X4& GetLocalTransform() const { return m_LocalTransform; }


	protected:
		/*OwnerShip�� ������ ���� ����*/
		TransformComponent* m_Parent = nullptr;
		DirectX::XMFLOAT4X4 m_WorldTransform = Matrix4x4::Identity();
		DirectX::XMFLOAT4X4 m_LocalTransform = Matrix4x4::Identity();
		float m_Scale = 1.0f;


		DirectX::XMFLOAT3 m_Velocity = { 0.0f, 0.0f,0.0f };
		float m_InverseMass = 10.0f;


		float m_Roll = 0.0f;
		float m_Pitch = 0.0f;
		float m_Yaw = 0.0f;

		DirectX::XMFLOAT3 m_Right = Vector3::UnitX;
		DirectX::XMFLOAT3 m_Up = Vector3::UnitY;
		DirectX::XMFLOAT3 m_Forward = Vector3::UnitZ;
	};

}