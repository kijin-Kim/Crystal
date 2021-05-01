#pragma once
#include <array>
#include "Crystal/Core/Logger.h"
#include "SmartPointerHelper.h"
#include <set>
#include <unordered_map>

namespace Crystal {
#define STATIC_TYPE_IMPLE(classType) virtual std::string StaticType() const { return #classType; }
#define MAX_OBJECT_OWNER_COUNT 5
	// Has name
	class Object : public std::enable_shared_from_this<Object>
	{
	public:
		Object() = default;
		virtual ~Object()
		{
			CS_DEBUG_INFO("Object : [%s]�� ���ŵǾ����ϴ�", m_Name.c_str());
		}

		virtual void OnCreate()
		{
			// ����Ʈ �̸� ����
			// ������Ʈ�� Ÿ���� ������� �̸��� ����
			// ������Ʈ�� ī��Ʈ�� ã�� �� ī��Ʈ�� ������� �̸��� ����
			
			if(m_Name.empty())
				SetDefaultName();

			CS_DEBUG_INFO("Object : [%s]�� �����Ǿ����ϴ�", m_Name.c_str());
		}

		


		void SetDefaultName()
		{
			m_Name = StaticType();
		}

		void SetObjectName(const std::string& name) 
		{
			m_Name = name; 
		}

		const std::string& GetObjectName() const { return m_Name; }
	

		void SetOwner(const std::weak_ptr<Object>& owner) { m_Owner = owner; }
		std::weak_ptr<Object> GetOwner() const { return m_Owner; }
		

		STATIC_TYPE_IMPLE(Object)

	private:
		std::string m_Name;
		std::weak_ptr<Object> m_Owner;
	};
	
	// Update-able
	class Updatable : public Object
	{
	public:
		Updatable() = default;
		~Updatable() override = default;

		virtual void Update(const float deltaTime) {}

		STATIC_TYPE_IMPLE(Updatable)
	};
}