#include "cspch.h"
#include "Blackboard.h"

namespace Crystal {


	void BlackboardComponent::Initialize()
	{
		m_Blackboard = CreateObject<Blackboard>();
	}

	void BlackboardComponent::SetValueAsBool(const std::string& key, bool value)
	{
		
		if (!m_Blackboard)
		{
			return;
		}


		auto newValue = new BlackBoardBoolValue();
		newValue->Value = value;
		
		m_Blackboard->m_Values[key] = newValue;
	}

	void BlackboardComponent::SetValueAsInt(const std::string& key, int value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		auto newValue = new BlackBoardIntValue();
		newValue->Value = value;

		m_Blackboard->m_Values[key] = newValue;
	}

	void BlackboardComponent::SetValueAsFloat(const std::string& key, float value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		auto newValue = new BlackBoardFloatValue();
		newValue->Value = value;

		m_Blackboard->m_Values[key] = newValue;
	}

	void BlackboardComponent::SetValueAsFloat3(const std::string& key, const DirectX::XMFLOAT3& value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		auto newValue = new BlackBoardFloat3Value();
		newValue->Value = value;

		m_Blackboard->m_Values[key] = newValue;
	}

	void BlackboardComponent::SetValueAsFloat4(const std::string& key, const DirectX::XMFLOAT4& value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		auto newValue = new BlackBoardFloat4Value();
		newValue->Value = value;

		m_Blackboard->m_Values[key] = newValue;
	}

	void BlackboardComponent::SetValueAsString(const std::string& key, const std::string& value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		auto newValue = new BlackBoardStringValue();
		newValue->Value = value;

		m_Blackboard->m_Values[key] = newValue;
	}

	void BlackboardComponent::SetValueAsObject(const std::string& key, Weak<Object> value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		auto newValue = new BlackBoardObjectValue();
		newValue->Value = value;

		m_Blackboard->m_Values[key] = newValue;
	}

	void BlackboardComponent::ClearValue(const std::string& key)
	{
		auto it = m_Blackboard->m_Values.find(key);
		if(it != m_Blackboard->m_Values.end())
		{
			delete it->second;
			m_Blackboard->m_Values.erase(it);
		}
	}

	bool BlackboardComponent::HasValue(const std::string& key)
	{
		return m_Blackboard->m_Values.count(key) != 0;
	}


	bool BlackboardComponent::GetValueAsBool(const std::string& key) const
	{
		if (!m_Blackboard)
		{
			return false;
		}
		
		auto it = m_Blackboard->m_Values.find(key);
		if(it != m_Blackboard->m_Values.end() && it->second)
		{
			return static_cast<BlackBoardBoolValue*>(it->second)->Value;
		}
		return false;
	}

	int BlackboardComponent::GetValueAsInt(const std::string& key) const
	{
		if (!m_Blackboard)
		{
			return 0;
		}
		
		auto it = m_Blackboard->m_Values.find(key);
		if (it != m_Blackboard->m_Values.end() && it->second)
		{
			return static_cast<BlackBoardIntValue*>(it->second)->Value;
		}

		return 0;
	}

	float BlackboardComponent::GetValueAsFloat(const std::string& key) const
	{
		if (!m_Blackboard)
		{
			return 0.0f;
		}

		
		auto it = m_Blackboard->m_Values.find(key);
		if (it != m_Blackboard->m_Values.end() && it->second)
		{
			return static_cast<BlackBoardFloatValue*>(it->second)->Value;
		}
		
		return 0.0f;
	}

	const DirectX::XMFLOAT3& BlackboardComponent::GetValueAsFloat3(const std::string& key) const
	{
		if (!m_Blackboard)
		{
			return Crystal::Vector3::Zero;
		}
		
		auto it = m_Blackboard->m_Values.find(key);
		if (it != m_Blackboard->m_Values.end() && it->second)
		{
			return static_cast<BlackBoardFloat3Value*>(it->second)->Value;
		}
		
		return Crystal::Vector3::Zero;
	}

	const DirectX::XMFLOAT4& BlackboardComponent::GetValueAsFloat4(const std::string& key) const
	{
		if (!m_Blackboard)
		{
			return Crystal::Vector4::Zero;
		}
		
		auto it = m_Blackboard->m_Values.find(key);
		if (it != m_Blackboard->m_Values.end() && it->second)
		{
			return static_cast<BlackBoardFloat4Value*>(it->second)->Value;
		}
		
		return Crystal::Vector4::Zero;
	}

	const std::string& BlackboardComponent::GetValueAsString(const std::string& key) const
	{
		if (!m_Blackboard)
		{
			return {};
		}
		
		auto it = m_Blackboard->m_Values.find(key);
		if (it != m_Blackboard->m_Values.end() && it->second)
		{
			return static_cast<BlackBoardStringValue*>(it->second)->Value;
		}
		
		return {};
	}

	Weak<Object> BlackboardComponent::GetValueAsObject(const std::string& key) const
	{
		if (!m_Blackboard)
		{
			return {};
		}
		
		auto it = m_Blackboard->m_Values.find(key);
		if (it != m_Blackboard->m_Values.end() && it->second)
		{
			return static_cast<BlackBoardObjectValue*>(it->second)->Value;
		}
		
		return {};
	}
}
