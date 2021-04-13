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
		void SetObjectOwner(std::weak_ptr<Object> object, int ownerType) { m_Owners[ownerType] = object; }

		const std::string& GetObjectName() const { return m_Name; }
		std::weak_ptr<Object> GetObjectOwner(int ownerType) const 
		{ 
			if (ownerType >= MAX_OBJECT_OWNER_COUNT)
			{
				CS_WARN("�߸��� ownerType�Դϴ�");
				return {};
			}
			return m_Owners[ownerType]; 
		}

		STATIC_TYPE_IMPLE(Object)

	private:
		std::string m_Name;
		std::array<std::weak_ptr<Object>, MAX_OBJECT_OWNER_COUNT> m_Owners = {};


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