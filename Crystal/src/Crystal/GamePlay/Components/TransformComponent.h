#pragma once
#include "Component.h"

namespace Crystal {

	/*물리적인 위치를 나타내는 특성을 가지고 있는 컴포넌트들의 베이스 클래스
MainComponent가 될 수 있고, 최대 하나의 부모를 가지고 부모의 상대적 Transform을 지님*/
	class TransformComponent : public Component
	{
	public:
		TransformComponent(const std::string& name) : Component(name)
		{
		}

		~TransformComponent() override = default;

		void Update(const float deltaTime) override;

		/*Component의 Transform이 다른 특정 Component에 종속되도록 합니다.*/
		void SetAttachment(TransformComponent* parentComponent)
		{
			/*서로가 서로의 Parent*/
			CS_FATAL(parentComponent->GetParentComponent() != this, "Component : %s 와 Component : %s는 서로가 서로의 부모입니다.",
				GetName().c_str(), parentComponent->GetName().c_str());
			CS_FATAL(GetOwner()->GetMainComponent(), "MainComponent가 존재 하지 않습니다. 먼저 MainComponent를 지정해주세요.",
				GetName().c_str());
			CS_FATAL(GetOwner()->GetMainComponent() != this, "Component : %s 는 MainComponent입니다 Maincomponent는 다른 Component에 Attach 될 수 없습니다.",
				GetName().c_str());

			Actor* owner = GetOwner();
			m_ParentComponent = parentComponent;

			/*현재 Component를 Transform Component Hierarchy로 이동시킵니다.*/
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


		void RotateRoll(float angle);
		void RotatePitch(float angle);
		void RotateYaw(float angle);
	

		DirectX::XMFLOAT3 GetLocalPosition() const { return { m_LocalTransform._41, m_LocalTransform._42, m_LocalTransform._43 }; }
		DirectX::XMFLOAT3 GetWorldPosition() const { return { m_WorldTransform._41, m_WorldTransform._42, m_WorldTransform._43 }; }
		const DirectX::XMFLOAT3& GetVelocity() const { return m_Velocity; }
		float GetMass() const
		{
			CS_FATAL(m_InverseMass != 0, "질량이 무한대 입니다. 먼저 HasFiniteMass()로 검사되어야합니다");
			return 1.0f / m_InverseMass;
		}
		float GetInverseMass() const { return m_InverseMass; }
		bool HasFiniteMass() const { return m_InverseMass != 0; }

	
		const DirectX::XMFLOAT3& GetRight() const { return m_Right; }
		const DirectX::XMFLOAT3& GetUp()  const { return m_Up; }
		const DirectX::XMFLOAT3& GetForward() const { return m_Forward; }

		TransformComponent* GetParentComponent() const { return m_ParentComponent; }
		const DirectX::XMFLOAT4X4& GetWorldTransform() const { return m_WorldTransform; }
		const DirectX::XMFLOAT4X4& GetLocalTransform() const { return m_LocalTransform; }





	protected:
		/*OwnerShip을 가지고 있지 않음*/
		TransformComponent* m_ParentComponent = nullptr;
		DirectX::XMFLOAT4X4 m_WorldTransform = Matrix4x4::Identity();
		DirectX::XMFLOAT4X4 m_LocalTransform = Matrix4x4::Identity();
		float m_Scale = 1.0f; // Unit Scale만 현재 허용함 ( Unit 이외의 Scale을 허용할 경우 조명이 적용되는 메쉬들의 노말 값을 보정해주어야 함 )


		DirectX::XMFLOAT3 m_Velocity = { 0.0f, 0.0f,0.0f };
		float m_InverseMass = 10.0f;

		DirectX::XMFLOAT4 m_Rotation = Vector4::Quaternion::Identity;

		DirectX::XMFLOAT3 m_Right = Vector3::UnitX;
		DirectX::XMFLOAT3 m_Up = Vector3::UnitY;
		DirectX::XMFLOAT3 m_Forward = Vector3::UnitZ;

	};

}
