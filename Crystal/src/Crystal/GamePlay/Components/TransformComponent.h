#pragma once
#include "Component.h"

namespace Crystal {
	/*
	 * 물리적인 위치를 나타내는 특성을 가지고 있는 컴포넌트들의 베이스 클래스
	 * MainComponent가 될 수 있고, 최대 하나의 부모를 가지고 부모의 상대적 Transform을 지님
	 * 다른 Transform Component에 계층구조로 붙을 수 있습니다.
	 */

	class TransformComponent : public Component
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Component>(*this);
			
			ar & m_WorldTransform;
			ar & m_LocalTransform;
			
			ar & m_Scale;

			ar & m_RotationQuat;

			ar & m_Velocity;
			ar & m_InverseMass;

			ar & m_Right;
			ar & m_Up;
			ar & m_Forward;

			ar& m_ForceAccum;
		}

	public:
		TransformComponent() = default;
		~TransformComponent() override = default;

		void Begin() override;

		void UpdateTransformByForce(float deltaTime);
		void Update(float deltaTime) override;
		virtual void UpdateTransforms();
		void UpdateLocalTransform();
		void UpdateWorldTransform();

		void AttachTo(const std::shared_ptr<TransformComponent> component);

		void SetLocalPosition(const DirectX::XMFLOAT3& position);
		void SetWorldPosition(const DirectX::XMFLOAT3& position);

		void SetUnitScale(const float scale);
		void SetVelocity(const DirectX::XMFLOAT3& velocity);
		void SetMass(const float mass);
		void SetInverseMass(const float inverseMass);

		void SetRotationQuat(const DirectX::XMFLOAT4& rotation);

		void SetWorldTransform(const DirectX::XMFLOAT4X4& transform);

		void RotateRoll(float angle);
		void RotatePitch(float angle);
		void RotateYaw(float angle);

		DirectX::XMFLOAT3 GetLocalPosition() const;
		DirectX::XMFLOAT3 GetWorldPosition();
		const DirectX::XMFLOAT3& GetVelocity() const;
		const DirectX::XMFLOAT3& GetScale();
		float GetMass() const;
		float GetInverseMass() const;
		bool HasFiniteMass() const;

		const DirectX::XMFLOAT3& GetLocalRightVector() const;
		const DirectX::XMFLOAT3& GetLocalUpVector() const;
		const DirectX::XMFLOAT3& GetLocalForwardVector() const;
		DirectX::XMFLOAT3 GetWorldRightVector();
		DirectX::XMFLOAT3 GetWorldUpVector();
		DirectX::XMFLOAT3 GetWorldForwardVector();

		void SetParentComponent(const std::weak_ptr<TransformComponent>& component);
		std::weak_ptr<TransformComponent> GetParentComponent() const;


		const DirectX::XMFLOAT4X4& GetWorldTransform();
		const DirectX::XMFLOAT4X4& GetLocalTransform() const;

		const DirectX::XMFLOAT4& GetRotationQuat() const;
		const DirectX::XMFLOAT3& GetRotation() const;


		bool CanBeRendered() const override;
		bool IsCollisionEnabled() const override;

		void AddForce(const DirectX::XMFLOAT3& force) { m_ForceAccum = Vector3::Add(m_ForceAccum, force); }
		const DirectX::XMFLOAT3& GetLastForceAccum() const { return m_LastForceAccum; }

		STATIC_TYPE_IMPLE(TransformComponent)

	protected:
		/*OwnerShip을 가지고 있지 않음*/
		std::weak_ptr<TransformComponent> m_ParentComponent;

		DirectX::XMFLOAT4X4 m_WorldTransform = Matrix4x4::Identity();
		DirectX::XMFLOAT4X4 m_LocalTransform = Matrix4x4::Identity();


		DirectX::XMFLOAT3 m_Scale = {1.0f, 1.0f, 1.0f }; // Unit Scale만 현재 허용함 ( Unit 이외의 Scale을 허용할 경우 조명이 적용되는 메쉬들의 노말 값을 보정해주어야 함 )
		DirectX::XMFLOAT4 m_RotationQuat = Vector4::Quaternion::Identity;
		


		DirectX::XMFLOAT3 m_Velocity = Vector3::Zero;
		float m_InverseMass = 1.0f;


		DirectX::XMFLOAT3 m_Right = Vector3::UnitX;
		DirectX::XMFLOAT3 m_Up = Vector3::UnitY;
		DirectX::XMFLOAT3 m_Forward = Vector3::UnitZ;

		
		DirectX::XMFLOAT3 m_ForceAccum = Vector3::Zero;
		DirectX::XMFLOAT3 m_LastForceAccum = Vector3::Zero;


	};
}
