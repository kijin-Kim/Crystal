#pragma once
#include <array>
#include "Crystal/Core/Logger.h"
#include "SmartPointerHelper.h"
#include <set>
#include <unordered_map>

namespace Crystal {
#define STATIC_TYPE_IMPLE(classType) virtual std::string StaticType() const { return #classType; }
	
	class Object : public std::enable_shared_from_this<Object>
	{
	public:
		Object();
		virtual ~Object();

		virtual void Initialize();
		virtual void OnCreate();


		virtual void Update(float deltaTime);

		void SetDefaultName();

		void SetObjectName(const std::string& name);

		const std::string& GetObjectName() const;


		void SetOuter(const std::weak_ptr<Object>& outer);
		std::weak_ptr<Object> GetOuter() const;

		template<class T>
		std::shared_ptr<T> CreateObject(const std::string& name = "", const std::weak_ptr<Object>& outer = {})
		{
			std::shared_ptr<T> newObject = std::make_shared<T>();
			if (!name.empty())
			{
				newObject->SetObjectName(name);
			}
			if(!outer.expired())
			{
				newObject->SetOuter(outer);
			}
			
			newObject->OnCreate();

			return newObject;
		}


		STATIC_TYPE_IMPLE(Object)

	private:
		std::string m_Name;
		std::weak_ptr<Object> m_Outer;
	};
	
}