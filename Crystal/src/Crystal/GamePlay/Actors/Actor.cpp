#include "cspch.h"
#include "Actor.h"

#include "Crystal/GamePlay/Components/BaseComponents.h"

namespace Crystal {

	void Actor::UpdateComponents(float deltaTime)
	{
		/*Non-Hierarchy Components + Main Component*/
		for (auto& component : m_Components)
			component->Update(deltaTime);
		/*Hierarchy Components 계산의 효율을 위해 최상위 부모 (MainComponent)부터 자식 순으로 transform이 계산됩니다.*/
		for (auto& transformComponent : m_TransformHierarchy)
			transformComponent->Update(deltaTime);
	}

	void Actor::RegisterComponent(Component* component)
	{
		component->SetOwner(this);
		if (m_Components.end() != std::find_if(m_Components.begin(), m_Components.end(), 
			[component](const std::unique_ptr<Component>& com)->bool {return com.get() == component;}))
		{
			CS_WARN("삽입하려는 Component : %s가 이미 존재합니다", component->GetName().c_str());
			return;
		}
		m_Components.emplace_back(component);

		CS_DEBUG_INFO("Component : %s Registered", component->GetName().c_str());
	}

	void Actor::UnRegisterComponent(Component* component)
	{
		component->SetOwner(nullptr);
		auto it = std::find_if(m_Components.begin(), m_Components.end(), 
			[component](const std::unique_ptr<Component>& com) ->bool {return com.get() == component; });
		if (it == m_Components.end())
		{
			CS_WARN("삭제하려는 Component : %s 가 존재하지 않습니다", component->GetName().c_str());
			return;
		}

		/*erase_copy를 방지하기 위하여 맨 뒤에 원소를 앞으로 보냅니다.*/
		CS_DEBUG_INFO("Component : %s Unregistered", component->GetName().c_str());
		m_Components.back().swap(*it);
		m_Components.erase(m_Components.end() - 1);
	}

	void Actor::MoveToTransformComponentHierarchy(TransformComponent* component)
	{
		/*Hierarchy에 이미 있는지 검사*/
		if (m_TransformHierarchy.end() != std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(), 
			[component](const std::unique_ptr<TransformComponent>& com) ->bool {return com.get() == component; }))
		{
			CS_WARN("삽입하려는 Component : %s가 이미 Transform Component Hierarchy에 존재합니다", component->GetName().c_str());
			return;
		}

		/*Component 배열에서 Move할 컴포넌트를 찾음*/
		auto componentIt = std::find_if(m_Components.begin(), m_Components.end(), 
			[component](const std::unique_ptr<Component>& com) ->bool {return com.get() == component; });
		if (componentIt == m_Components.end())
		{
			CS_WARN("Move하려는 Component : %s 가 존재하지 않습니다", component->GetName().c_str());
			return;
		}

		/*erase_copy를 방지하기 위하여 맨 뒤에 원소를 앞으로 보냅니다.*/
		m_Components.back().swap(*componentIt);

		/*Move!*/
		std::unique_ptr<TransformComponent> moveDestination(static_cast<TransformComponent*>((*componentIt).release()));

		/*Relase되고 남은 nullptr를 삭제합니다*/
		m_Components.erase(m_Components.end() - 1);

		
		
		/*Transform Hierarchy에서의 위치를 찾아 삽입합니다..*/
		auto it = std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(), 
			[component](const std::unique_ptr<TransformComponent>& com) ->bool {return com.get() == component->GetParent(); });
		/*부모를 찾았을 시*/
		if (it != m_TransformHierarchy.end())
		{
			/*부모 바로 다음 자리에 삽입*/
			m_TransformHierarchy.insert(it + 1, std::move(moveDestination));
		}
		/*부모를 못 찾았을 시 == MainComponent의 자식*/
		else
		{
			m_TransformHierarchy.insert(m_TransformHierarchy.begin(), std::move(moveDestination));
		}

		CS_DEBUG_INFO("Component : %s Moved", component->GetName().c_str());
	}

}