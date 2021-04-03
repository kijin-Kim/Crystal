#include "cspch.h"
#include "Actor.h"

#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	void Actor::OnCreate()
	{
		Updatable::OnCreate();

		CS_FATAL(m_MainComponent, "Actor : %s�� MainComponent�� nullptr �Դϴ�", GetObjectName().c_str());


		for (const auto& c : m_Components)
		{
			if (!c->CanBeRendered())
				continue;

			Renderer::Instance().RegisterRendererComponent(Cast<PrimitiveComponent>(c));
		}

		for (const auto& tc : m_TransformHierarchy)
		{
			if(!tc->CanBeRendered())
				continue;

			Renderer::Instance().RegisterRendererComponent(Cast<PrimitiveComponent>(tc));
		}
		
	}

	void Actor::UpdateComponents(float deltaTime)
	{
		/*Non-Hierarchy Components + Main Component*/
		for (auto& component : m_Components)
			component->Update(deltaTime);
		/*Hierarchy Components ����� ȿ���� ���� �ֻ��� �θ� (MainComponent)���� �ڽ� ������ transform�� ���˴ϴ�.*/
		for (auto& transformComponent : m_TransformHierarchy)
			transformComponent->Update(deltaTime);
	}

	void Actor::RegisterComponent(const std::shared_ptr<Component>& component)
	{
		if (m_Components.end() != std::find_if(m_Components.begin(), m_Components.end(),
			[component](const std::shared_ptr<Component>& com)->bool {return com == component; }))
		{
			CS_WARN("�����Ϸ��� Component : %s�� �̹� �����մϴ�", component->GetObjectName().c_str());
			return;
		}
		m_Components.emplace_back(component);

		CS_DEBUG_INFO("Component : %s Registered", component->GetObjectName().c_str());
	}

	void Actor::MoveToTransformComponentHierarchy(const std::shared_ptr <TransformComponent>& component)
	{
		/*Hierarchy�� �̹� �ִ��� �˻�*/
		if (m_TransformHierarchy.end() != std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
			[component](const std::shared_ptr<TransformComponent>& com) ->bool {return com == component; }))
		{
			CS_WARN("�����Ϸ��� Component : %s�� �̹� Transform Component Hierarchy�� �����մϴ�", component->GetObjectName().c_str());
			return;
		}

		/*Component �迭���� Move�� ������Ʈ�� ã��*/
		auto componentIt = std::find_if(m_Components.begin(), m_Components.end(),
			[component](const std::shared_ptr<Component>& com) ->bool {return com == component; });
		if (componentIt == m_Components.end())
		{
			CS_WARN("Move�Ϸ��� Component : %s �� �������� �ʽ��ϴ�", component->GetObjectName().c_str());
			return;
		}

		// ������ �迭���� erase �մϴ�.
		m_Components.erase(componentIt);

		/*Transform Hierarchy������ ��ġ�� ã�� �����մϴ�..*/
		auto it = std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
			[component](const std::shared_ptr<TransformComponent>& com) ->bool {return com.get() == component->GetParentComponent(); });
		/*�θ� ã���� ��*/
		if (it != m_TransformHierarchy.end())
		{
			/*�θ� �ٷ� ���� �ڸ��� ����*/
			m_TransformHierarchy.insert(it + 1, component);
		}
		/*�θ� �� ã���� �� == MainComponent�� �ڽ�*/
		else
		{
			m_TransformHierarchy.insert(m_TransformHierarchy.begin(), component);
		}
		CS_DEBUG_INFO("Component : %s Moved", component->GetObjectName().c_str());
	}

	void Actor::SetPosition(const DirectX::XMFLOAT3& position)
	{
		m_MainComponent->SetLocalPosition(position);
	}

	void Actor::SetAttachment(const std::shared_ptr<TransformComponent>& from, const std::shared_ptr<TransformComponent>& to)
	{
		from->SetParentComponent(to);
		MoveToTransformComponentHierarchy(from);
	}

}