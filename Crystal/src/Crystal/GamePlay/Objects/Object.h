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
			CS_DEBUG_INFO("Object : [ %s ]가 제거되었습니다", m_Name.c_str());
		}

		virtual void OnCreate()
		{
			// 디폴트 이름 설정
			// 오브젝트의 타입을 기반으로 이름을 설정
			// 오브젝트의 카운트를 찾고 그 카운트를 기반으로 이름을 설정
			
			SetDefaultName();

			CS_DEBUG_INFO("Object : [ %s ]가 생성되었습니다", m_Name.c_str());
			s_ObjectCreationCountTracker[m_Name]++;
		}


		void SetDefaultName()
		{
			m_Name = StaticType() + "_";
			m_Name += std::to_string(s_ObjectCreationCountTracker[m_Name]++);
		}

		void SetObjectName(const std::string& name) 
		{
			auto it = s_ObjectNameTracker.find(name);
			if (it != s_ObjectNameTracker.end())
			{
				return;
			}

			s_ObjectNameTracker.insert(name);
			m_Name = name; 
		}
		void SetObjectOwner(std::weak_ptr<Object> object, int ownerType) { m_Owners[ownerType] = object; }

		const std::string& GetObjectName() const { return m_Name; }
		std::weak_ptr<Object> GetObjectOwner(int ownerType) const 
		{ 
			if (ownerType >= MAX_OBJECT_OWNER_COUNT)
			{
				CS_WARN("잘못된 ownerType입니다");
				return {};
			}
			return m_Owners[ownerType]; 
		}

		STATIC_TYPE_IMPLE(Object)

	private:
		std::string m_Name;
		std::array<std::weak_ptr<Object>, MAX_OBJECT_OWNER_COUNT> m_Owners = {};


		using ObjectCountTracker = std::unordered_map<std::string, int>;
		using ObjectNameTracker = std::set<std::string>;

		/*단순히 오브젝트가 Create된 횟수를 셉니다. (※ 주의 : 현재 살아있는 오브젝트의 수가 아님.)*/
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