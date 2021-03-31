#pragma once
#include <array>
#include "Crystal/Core/Logger.h"
#include <set>
#include <unordered_map>

namespace Crystal {
#define STATIC_TYPE_IMPLE(classType) virtual std::string StaticType() const { return #classType; }

	// Has name
	class Object
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
			CS_DEBUG_INFO("Object : [ %s ]가 제거되었습니다", m_Name.c_str());
		}

		virtual void OnCreate()
		{
			// 디폴트 이름 설정
			// 오브젝트의 타입을 기반으로 이름을 설정
			// 오브젝트의 카운트를 찾고 그 카운트를 기반으로 이름을 설정
			m_Name = StaticType() + "_";
			m_Name += std::to_string(s_ObjectCountTracker[m_Name]++);
			
			CS_DEBUG_INFO("Object : [ %s ]가 생성되었습니다", m_Name.c_str());

			s_ObjectCountTracker[m_Name]++;
		}

		void SetObjectName(const std::string& name) 
		{
			auto it = s_ObjectNameTracker.find(name);
			if (it != s_ObjectNameTracker.end())
			{
				CS_WARN("[ %s ]이름을 가진 Object가 이미 존재합니다", name.c_str());
				return;
			}

			s_ObjectNameTracker.insert(name);
			m_Name = name; 
		}
		void SetObjectOwner(Object* object, ObjectOwnerType ownerType) { m_Owners[ownerType] = object; }

		const std::string& GetObjectName() const { return m_Name; }
		Object* GetObjectOwner(ObjectOwnerType ownerType) const { return m_Owners[ownerType]; }

		template<class T>
		static std::unique_ptr<T> CreateUniqueObject()
		{
			std::unique_ptr<T> newObject = std::make_unique<T>();
			newObject->OnCreate();
			return newObject;
		}

		template<class T>
		static std::shared_ptr<T> CreateSharedObject(const std::string& name)
		{
			std::shared_ptr<T> newObject = std::make_shared<T>();
			newObject->OnCreate();
			return newObject;
		}

		STATIC_TYPE_IMPLE(Object)

	private:
		std::string m_Name;
		std::array<Object*, Object_Owner_Type_Count> m_Owners = {};


		using ObjectCountTracker = std::unordered_map<std::string, int>;
		using ObjectNameTracker = std::set<std::string>;

		static ObjectCountTracker  s_ObjectCountTracker;
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