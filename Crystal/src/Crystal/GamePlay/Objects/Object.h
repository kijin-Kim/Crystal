#pragma once
#include <array>
#include "Crystal/Core/Logger.h"
#include "SmartPointerHelper.h"
#include <set>
#include <unordered_map>

namespace Crystal {
#define STATIC_TYPE_IMPLE(classType) virtual std::string StaticType() const { return #classType; }

	// Has name
	class Object : public std::enable_shared_from_this<Object>
	{
	public:
		enum ObjectOwnerType
		{
			OOT_Level = 0,
			OOT_World,
			OOT_Actor,
			OOT_Component,
			OOT_Spawner,
			Object_Owner_Type_Count
		};
	public:
		Object() = default;
		virtual ~Object()
		{
			CS_DEBUG_INFO("Object : [ %s ]�� ���ŵǾ����ϴ�", m_Name.c_str());
		}

		virtual void OnCreate()
		{
			// ����Ʈ �̸� ����
			// ������Ʈ�� Ÿ���� ������� �̸��� ����
			// ������Ʈ�� ī��Ʈ�� ã�� �� ī��Ʈ�� ������� �̸��� ����
			m_Name = StaticType() + "_";
			m_Name += std::to_string(s_ObjectCreationCountTracker[m_Name]++);
			
			CS_DEBUG_INFO("Object : [ %s ]�� �����Ǿ����ϴ�", m_Name.c_str());
			s_ObjectCreationCountTracker[m_Name]++;
		}


		void SetObjectName(const std::string& name) 
		{
			auto it = s_ObjectNameTracker.find(name);
			if (it != s_ObjectNameTracker.end())
			{
				CS_WARN("[ %s ]�̸��� ���� Object�� �̹� �����մϴ�", name.c_str());
				return;
			}

			s_ObjectNameTracker.insert(name);
			m_Name = name; 
		}
		void SetObjectOwner(std::weak_ptr<Object> object, ObjectOwnerType ownerType) { m_Owners[ownerType] = object; }

		const std::string& GetObjectName() const { return m_Name; }
		std::weak_ptr<Object> GetObjectOwner(ObjectOwnerType ownerType) const { return m_Owners[ownerType]; }

		STATIC_TYPE_IMPLE(Object)

	private:
		std::string m_Name;
		std::array<std::weak_ptr<Object>, Object_Owner_Type_Count> m_Owners = {};


		using ObjectCountTracker = std::unordered_map<std::string, int>;
		using ObjectNameTracker = std::set<std::string>;

		/*�ܼ��� ������Ʈ�� Create�� Ƚ���� ���ϴ�. (�� ���� : ���� ����ִ� ������Ʈ�� ���� �ƴ�.)*/
		static ObjectCountTracker  s_ObjectCreationCountTracker; 
		static ObjectNameTracker s_ObjectNameTracker;

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