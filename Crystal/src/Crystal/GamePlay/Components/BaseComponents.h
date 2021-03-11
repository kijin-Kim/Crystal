#pragma once
#include <DirectXMath.h>
#include "Crystal/Math/Math.h"
#include "Crystal/GamePlay/Actors/Actor.h"
#include "../../Core/Logger.h"
#include "../../Renderer/Renderable.h"

namespace Crystal {
	class Component
	{
	public:
		Component(const std::string& name) : m_Name(name) {}
		virtual ~Component() = default;

		virtual void Update(float deltaTime) {}

		void SetOwner(Actor* owner) { m_Owner = owner; }
		Actor* GetOwner() const { if (!m_Owner) CS_WARN("Owner가 nullptr입니다"); return m_Owner; }


		const std::string& GetName() const { return m_Name; }

	private:
		/*OwnerShip을 가지고 있지 않음*/
		Actor* m_Owner = nullptr;
		std::string m_Name;
	};


	/*물리적인 위치를 나타내는 특성을 가지고 있는 컴포넌트들의 베이스 클래스
	MainComponent가 될 수 있고, 최대 하나의 부모를 가지고 부모의 상대적 Transform을 지님*/
	class TransformComponent : public Component
	{
	public:
		TransformComponent(const std::string& name) : Component(name)
		{
			DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());
		}
		virtual ~TransformComponent()
		{
		}

		virtual void Update(float deltaTime) override
		{
			Component::Update(deltaTime);
			
			auto right = GetRight();
			auto up = GetUp();
			auto forward = GetForward();
			auto position = GetPosition();

			m_Transform = {
				right.x, right.y , right.z, 0.0f,
				up.x, up.y, up.z, 0.0f,
				forward.x, forward.y, forward.z, 0.0f,
				position.x, position.y, position.z, 1.0f
			};

			DirectX::XMFLOAT4X4 scale = Matrix4x4::Scale({ m_Scale, m_Scale, m_Scale });
			m_Transform = Matrix4x4::Multiply(scale, m_Transform);
		


			/*Parent부터 자식 순으로 계산하여야 함*/
			if (m_Parent)
				m_Transform = Matrix4x4::Multiply(m_Parent->GetTransform(), m_Transform);
		}

		/*Component의 Transform이 다른 특정 Component에 종속되도록 합니다.*/
		void AttachToComponent(TransformComponent* parentComponent) 
		{
			/*서로가 서로의 Parent*/
			CS_FATAL(parentComponent->GetParent() != this, "Component : %s 와 Component : %s는 서로가 서로의 부모입니다.",
				GetName().c_str(), parentComponent->GetName().c_str());
			CS_FATAL(GetOwner()->GetMainComponent(), "MainComponent가 존재 하지 않습니다. 먼저 MainComponent를 지정해주세요.",
				GetName().c_str());
			CS_FATAL(GetOwner()->GetMainComponent() != this, "Component : %s 는 MainComponent입니다 Maincomponent는 다른 Component에 Attach 될 수 없습니다.",
				GetName().c_str());

			Actor* owner = GetOwner();
			m_Parent = parentComponent;

			/*현재 Component를 Transform Component Hierarchy로 이동시킵니다.*/
			owner->MoveToTransformComponentHierarchy(this);
			
		}

		void SetPosition(const DirectX::XMFLOAT3& position) { m_Position = position; }
		void SetScale(float scale) { m_Scale = scale; }
		void SetVelocity(const DirectX::XMFLOAT3& velocity) { m_Velocity = velocity; }
		void SetMass(float mass) { m_Mass = mass; }

		void RotateRoll(float roll) { m_RollPitchYaw.x += roll; }
		void RotatePitch(float pitch) { m_RollPitchYaw.y += pitch; }
		void RotateYaw(float yaw) { m_RollPitchYaw.z += yaw; }
		void RotateRollPitchYaw(const DirectX::XMFLOAT3& rollPitchYaw)
		{
			m_RollPitchYaw = Vector3::Add(m_RollPitchYaw, rollPitchYaw);
		}

		const DirectX::XMFLOAT3 GetPosition() const { return m_Position; }
		const DirectX::XMFLOAT3 GetVelocity() const { return m_Velocity; }
		float GetMass() const { return m_Mass; }
		const DirectX::XMFLOAT3 GetRight() const { return Vector3::RotateQuaternion({ 1.0f, 0.0f, 0.0f }, GetOrientation()); }
		const DirectX::XMFLOAT3 GetUp() const { return Vector3::RotateQuaternion({ 0.0f, 1.0f, 0.0f }, GetOrientation()); }
		const DirectX::XMFLOAT3 GetForward() const { return Vector3::RotateQuaternion({ 0.0f, 0.0f, 1.0f }, GetOrientation()); }
		const DirectX::XMFLOAT4 GetOrientation() const { return Vector4::QuaternionRollPitchYaw(m_RollPitchYaw); }




		TransformComponent* GetParent() const { return m_Parent; }

		const DirectX::XMFLOAT4X4& GetTransform() const { return m_Transform; }


	private:
		/*OwnerShip을 가지고 있지 않음*/
		TransformComponent* m_Parent = nullptr;
		DirectX::XMFLOAT4X4 m_Transform = Matrix4x4::Identity();
		DirectX::XMFLOAT3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Scale = 1.0f;
		DirectX::XMFLOAT3 m_Velocity = { 0.0f, 0.0f,0.0f };
		float m_Mass = 10.0f;

		DirectX::XMFLOAT3 m_RollPitchYaw = { 0.0f, 0.0f, 0.0f };
	};


	/*물리적인 위치와 눈에 보이는 렌더링 가능한 오브젝트를 가지고 있는 컴포넌트들의 베이스 클래스*/
	class RenderComponent : public TransformComponent
	{
	public:
		enum class ERenderComponentType
		{
			None,
			Mesh,
			StaticMesh,
			SkeletalMesh,
		};

	public:
		RenderComponent(const std::string& name);
		virtual ~RenderComponent() = default;

		virtual void Update(float deltaTime) override
		{
			TransformComponent::Update(deltaTime);

			m_Renderable->Update(deltaTime);
		}
		
		ERenderComponentType GetRenderComponentType() const { return m_RenderComponentType; }

		void SetRenderable(Renderable* renderable) { m_Renderable = renderable; }
		const Renderable* GetRenderable() const { return m_Renderable; }

	protected:
		Renderable* m_Renderable = nullptr;
		ERenderComponentType m_RenderComponentType = ERenderComponentType::None;
	};
}
