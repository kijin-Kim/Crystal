#include "cspch.h"
#include "Actor.h"

#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/GamePlay/World/World.h"

namespace Crystal {
	void Actor::UpdateComponents(float deltaTime)
	{
		/*Non-Hierarchy Components + Main Component*/
		for (auto& component : m_Components)
			component->Update(deltaTime);
		/*Hierarchy Components ����� ȿ���� ���� �ֻ��� �θ� (MainComponent)���� �ڽ� ������ transform�� ���˴ϴ�.*/
		for (auto& transformComponent : m_TransformHierarchy)
			transformComponent->Update(deltaTime);
	}

	void Actor::RegisterComponent(Component* component)
	{
		component->SetOwner(this);
		if (m_Components.end() != std::find_if(m_Components.begin(), m_Components.end(),
			[component](const std::unique_ptr<Component>& com)->bool {return com.get() == component; }))
		{
			CS_WARN("�����Ϸ��� Component : %s�� �̹� �����մϴ�", component->GetObjectName().c_str());
			return;
		}
		m_Components.emplace_back(component);

		CS_DEBUG_INFO("Component : %s Registered", component->GetObjectName().c_str());
	}

	void Actor::UnRegisterComponent(Component* component)
	{
		component->SetOwner(nullptr);
		auto it = std::find_if(m_Components.begin(), m_Components.end(),
			[component](const std::unique_ptr<Component>& com) ->bool {return com.get() == component; });
		if (it == m_Components.end())
		{
			CS_WARN("�����Ϸ��� Component : %s �� �������� �ʽ��ϴ�", component->GetObjectName().c_str());
			return;
		}

		/*erase_copy�� �����ϱ� ���Ͽ� �� �ڿ� ���Ҹ� ������ �����ϴ�.*/
		CS_DEBUG_INFO("Component : %s Unregistered", component->GetObjectName().c_str());
		m_Components.back().swap(*it);
		m_Components.erase(m_Components.end() - 1);
	}

	void Actor::MoveToTransformComponentHierarchy(TransformComponent* component)
	{
		/*Hierarchy�� �̹� �ִ��� �˻�*/
		if (m_TransformHierarchy.end() != std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
			[component](const std::unique_ptr<TransformComponent>& com) ->bool {return com.get() == component; }))
		{
			CS_WARN("�����Ϸ��� Component : %s�� �̹� Transform Component Hierarchy�� �����մϴ�", component->GetObjectName().c_str());
			return;
		}

		/*Component �迭���� Move�� ������Ʈ�� ã��*/
		auto componentIt = std::find_if(m_Components.begin(), m_Components.end(),
			[component](const std::unique_ptr<Component>& com) ->bool {return com.get() == component; });
		if (componentIt == m_Components.end())
		{
			CS_WARN("Move�Ϸ��� Component : %s �� �������� �ʽ��ϴ�", component->GetObjectName().c_str());
			return;
		}

		/*erase_copy�� �����ϱ� ���Ͽ� �� �ڿ� ���Ҹ� ������ �����ϴ�.*/
		auto emptyMovedComponentIndex = componentIt - m_Components.begin();

		/*Move!*/
		std::unique_ptr<TransformComponent> moveDestination(static_cast<TransformComponent*>((*componentIt).release()));

		/*Relase�ǰ� ���� nullptr�� �����մϴ�*/
		m_Components.erase(m_Components.begin() + emptyMovedComponentIndex);

		/*Transform Hierarchy������ ��ġ�� ã�� �����մϴ�..*/
		auto it = std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
			[component](const std::unique_ptr<TransformComponent>& com) ->bool {return com.get() == component->GetParentComponent(); });
		/*�θ� ã���� ��*/
		if (it != m_TransformHierarchy.end())
		{
			/*�θ� �ٷ� ���� �ڸ��� ����*/
			m_TransformHierarchy.insert(it + 1, std::move(moveDestination));
		}
		/*�θ� �� ã���� �� == MainComponent�� �ڽ�*/
		else
		{
			m_TransformHierarchy.insert(m_TransformHierarchy.begin(), std::move(moveDestination));
		}

		CS_DEBUG_INFO("Component : %s Moved", component->GetObjectName().c_str());
	}

	void Actor::SetPosition(const DirectX::XMFLOAT3& position)
	{
		m_MainComponent->SetLocalPosition(position);
	}
}