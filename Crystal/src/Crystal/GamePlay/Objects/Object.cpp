#include "cspch.h"
#include "Object.h"


namespace Crystal {

	Object::Object()
		: m_Name(StaticType())
	{
	}

	Object::~Object()
	{
		CS_DEBUG_INFO("Object : [%s]가 제거되었습니다", m_Name.c_str());
	}

	void Object::Initialize()
	{
	}

	void Object::OnCreate()
	{
		CS_DEBUG_INFO("Object : [%s]가 생성되었습니다", m_Name.c_str());
	}

	void Object::Update(float deltaTime)
	{
	}

	void Object::SetDefaultName()
	{
		m_Name = StaticType();
	}

	void Object::SetObjectName(const std::string& name)
	{
		m_Name = name;
	}

	const std::string& Object::GetObjectName() const
	{
		return m_Name;
	}

	void Object::SetOuter(const std::weak_ptr<Object>& outer)
	{
		m_Outer = outer;
	}

	std::weak_ptr<Object> Object::GetOuter() const
	{
		return m_Outer;
	}
}


