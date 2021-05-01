#include "cspch.h"
#include "Actor.h"

#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/GamePlay/World/World.h"


namespace Crystal {

	
	void Actor::Initialize()
	{
		m_MainComponent = CreateComponent<TransformComponent>("DefaultMain");
	}

	void Actor::OnCreate()
	{
		Object::OnCreate();
		RegisterComponents();
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

	void Actor::AddComponent(const std::shared_ptr<Component>& component)
	{
		if (m_Components.end() != std::find_if(m_Components.begin(), m_Components.end(),
		                                       [component](const std::shared_ptr<Component>& com)-> bool
		                                       {
			                                       return com == component;
		                                       }))
		{
			CS_WARN("�����Ϸ��� Component : %s�� �̹� �����մϴ�", component->GetObjectName().c_str());
			return;
		}
		m_Components.emplace_back(component);

		CS_DEBUG_INFO("Component : %s Registered", component->GetObjectName().c_str());
	}

	void Actor::RegisterComponents()
	{
		CS_FATAL(m_MainComponent, "Actor : %s�� MainComponent�� nullptr �Դϴ�", GetObjectName().c_str());

		for (const auto& c : m_Components)
		{
			c->RegisterComponent();
		}

		for (const auto& tc : m_TransformHierarchy)
		{
			tc->RegisterComponent();
		}
	}

	void Actor::MoveToTransformComponentHierarchy(const std::shared_ptr<TransformComponent>& component)
	{
		CS_FATAL(!component->GetParentComponent().expired(), "�̵��Ϸ��� ������Ʈ�� Parent�� ���� ���� �ʽ��ϴ�.");

		
		/*Hierarchy�� �̹� �ִ��� �˻�*/
		if (m_TransformHierarchy.end() != std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
		                                               [component](
		                                               const std::shared_ptr<TransformComponent>& com) -> bool
		                                               {
			                                               return com == component;
		                                               }))
		{
			CS_WARN("�����Ϸ��� Component : %s�� �̹� Transform Component Hierarchy�� �����մϴ�",
			        component->GetObjectName().c_str());
			return;
		}

		/*Component �迭���� Move�� ������Ʈ�� ã��*/
		auto componentIt = std::find_if(m_Components.begin(), m_Components.end(),
		                                [component](const std::shared_ptr<Component>& com) -> bool
		                                {
			                                return com == component;
		                                });
		if (componentIt == m_Components.end())
		{
			CS_WARN("Move�Ϸ��� Component : %s �� �������� �ʽ��ϴ�", component->GetObjectName().c_str());
			return;
		}

		// ������ �迭���� erase �մϴ�.
		m_Components.erase(componentIt);


	
		/*Transform Hierarchy������ ��ġ�� ã�� �����մϴ�..*/
		auto it = std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
		                       [component](const std::shared_ptr<TransformComponent>& com) -> bool
		                       {
			                       return com.get() == component->GetParentComponent().lock().get();
		                       });


		/*�θ� ã���� ��*/
		if (it != m_TransformHierarchy.end())
		{
			/*�θ� �ٷ� ���� �ڸ��� ����*/
			m_TransformHierarchy.insert(it + 1, component);
		}
			/*�θ� �� ã���� �� == MainComponent�� �ڽ�*/
		else if (m_MainComponent == component->GetParentComponent().lock())
		{
			m_TransformHierarchy.insert(m_TransformHierarchy.begin(), component);
		}
		CS_DEBUG_INFO("Component : %s Moved", component->GetObjectName().c_str());
	}

	void Actor::SetPosition(const DirectX::XMFLOAT3& position)
	{
		m_MainComponent->SetLocalPosition(position);
	}

	std::weak_ptr<Component> Actor::GetComponentByName(const std::string& name)
	{
		auto componentIt = std::find_if(m_Components.begin(), m_Components.end(),
		                                [name](const std::shared_ptr<Component>& other)-> bool
		                                {
			                                return other->GetObjectName() == name;
		                                });
		if (componentIt != m_Components.end())
		{
			return *componentIt;
		}

		auto transformIt = std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
		                                [name](const std::shared_ptr<TransformComponent>& other)-> bool
		                                {
			                                return other->GetObjectName() == name;
		                                });

		if (transformIt != m_TransformHierarchy.end())
		{
			return *transformIt;
		}

		return {};
	}

	std::weak_ptr<Component> Actor::GetComponentByClass(const std::string& classType)
	{
		auto componentIt = std::find_if(m_Components.begin(), m_Components.end(),
		                                [classType](const std::shared_ptr<Component>& other)-> bool
		                                {
			                                return other->StaticType() == classType;
		                                });
		if (componentIt != m_Components.end())
		{
			return *componentIt;
		}

		auto transformIt = std::find_if(m_TransformHierarchy.begin(), m_TransformHierarchy.end(),
		                                [classType](const std::shared_ptr<TransformComponent>& other)-> bool
		                                {
			                                return other->StaticType() == classType;
		                                });
		if (transformIt != m_TransformHierarchy.end())
		{
			return *transformIt;
		}

		return {};
	}

	void Actor::SetAttachment(const std::shared_ptr<TransformComponent>& from,
	                          const std::shared_ptr<TransformComponent>& to)
	{
		from->SetParentComponent(to);
		MoveToTransformComponentHierarchy(from);
	}

}
