#include "cspch.h"
#include "Actor.h"

#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	void Actor::OnCreate()
	{
		Updatable::OnCreate();

		CS_FATAL(m_MainComponent, "Actor : %s의 MainComponent가 nullptr 입니다", GetObjectName().c_str());


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
		/*Hierarchy Components 계산의 효율을 위해 최상위 부모 (MainComponent)부터 자식 순으로 transform이 계산됩니다.*/
		for (auto& transformComponent : m_TransformHierarchy)
			transformComponent->Update(deltaTime);
	}

	void Actor::RegisterComponent(const std::shared_ptr<Component>& component)
	{
		if (m_Components.end() != std::find_if(m_Components.begin(), m_Components.end(),
			[component](const std::shared_ptr<Component>& com)->bool {return com == component; }))
		{
			CS_WARN("삽입하려는 Component : %s가 이미 존재합니다", component->GetObjectName().c_str());
			return;
		}
		m_Components.emplace_back(component);

		CS_DEBUG_INFO("Component : %s Registered", component->GetObjectName().c_str());
	}

	void Actor::MoveToTransformComponentHierarchy(const std::shared_ptr <TransformComponent>& component)
	{
		/*Hierarchy에 이미 있는지 검사*/
		if (m_TransformHierarchy.end() != std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
			[component](const std::shared_ptr<TransformComponent>& com) ->bool {return com == component; }))
		{
			CS_WARN("삽입하려는 Component : %s가 이미 Transform Component Hierarchy에 존재합니다", component->GetObjectName().c_str());
			return;
		}

		/*Component 배열에서 Move할 컴포넌트를 찾음*/
		auto componentIt = std::find_if(m_Components.begin(), m_Components.end(),
			[component](const std::shared_ptr<Component>& com) ->bool {return com == component; });
		if (componentIt == m_Components.end())
		{
			CS_WARN("Move하려는 Component : %s 가 존재하지 않습니다", component->GetObjectName().c_str());
			return;
		}

		// 기존의 배열에서 erase 합니다.
		m_Components.erase(componentIt);

		/*Transform Hierarchy에서의 위치를 찾아 삽입합니다..*/
		auto it = std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
			[component](const std::shared_ptr<TransformComponent>& com) ->bool {return com.get() == component->GetParentComponent(); });
		/*부모를 찾았을 시*/
		if (it != m_TransformHierarchy.end())
		{
			/*부모 바로 다음 자리에 삽입*/
			m_TransformHierarchy.insert(it + 1, component);
		}
		/*부모를 못 찾았을 시 == MainComponent의 자식*/
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