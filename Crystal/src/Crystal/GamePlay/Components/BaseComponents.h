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
		Actor* GetOwner() const { if (!m_Owner) CS_WARN("Owner�� nullptr�Դϴ�"); return m_Owner; }


		const std::string& GetName() const { return m_Name; }

	private:
		/*OwnerShip�� ������ ���� ����*/
		Actor* m_Owner = nullptr;
		std::string m_Name;
	};


	/*�������� ��ġ�� ��Ÿ���� Ư���� ������ �ִ� ������Ʈ���� ���̽� Ŭ����
	MainComponent�� �� �� �ְ�, �ִ� �ϳ��� �θ� ������ �θ��� ����� Transform�� ����*/
	class TransformComponent : public Component
	{
	public:
		TransformComponent(const std::string& name) : Component(name)
		{
			DirectX::XMStoreFloat4x4(&m_WorldTransform, DirectX::XMMatrixIdentity());
		}
		virtual ~TransformComponent()
		{
		}

		virtual void Update(float deltaTime) override
		{
			Component::Update(deltaTime);

			/*Parent���� �ڽ� ������ ����Ͽ��� ��*/
			if (m_Parent)
				m_WorldTransform = Matrix4x4::Multiply(m_Parent->GetWorldTransform(), m_LocalTransform);
			else
				m_WorldTransform = m_LocalTransform;
		}

		/*Component�� Transform�� �ٸ� Ư�� Component�� ���ӵǵ��� �մϴ�.*/
		void AttachToComponent(TransformComponent* parentComponent) 
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

		TransformComponent* GetParent() const { return m_Parent; }

		void SetLocalTransform(const DirectX::XMFLOAT4X4& transform) { m_LocalTransform = transform; }
		void SetWorldTransform(const DirectX::XMFLOAT4X4& transform) { m_WorldTransform = transform; }

		const DirectX::XMFLOAT4X4& GetLocalTransform() const { return m_LocalTransform; }
		const DirectX::XMFLOAT4X4& GetWorldTransform() const { return m_WorldTransform; }


	private:
		/*OwnerShip�� ������ ���� ����*/
		TransformComponent* m_Parent = nullptr;
		DirectX::XMFLOAT4X4 m_WorldTransform = Matrix4x4::Identity();
		DirectX::XMFLOAT4X4 m_LocalTransform = Matrix4x4::Identity();
	};


	/*�������� ��ġ�� ���� ���̴� ������ ������ ������Ʈ�� ������ �ִ� ������Ʈ���� ���̽� Ŭ����*/
	class RenderComponent : public TransformComponent
	{
	public:
		RenderComponent(const std::string& name) : TransformComponent(name) {}
		virtual ~RenderComponent() = default;

		virtual void Update(float deltaTime) override
		{
			TransformComponent::Update(deltaTime);
			for (const auto renderable : m_Renderables)
			{
				renderable->Update(deltaTime);
			}
		}

		const std::vector<Renderable*>& GetRenderables() const { return m_Renderables; }
		const Renderable* GetRenderable() const { return m_Renderables[0]; }

	protected:
		std::vector<Renderable*> m_Renderables;
	};
}
