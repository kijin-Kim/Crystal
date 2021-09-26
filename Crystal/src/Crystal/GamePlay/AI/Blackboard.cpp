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

		ClearValue(key);



		m_Blackboard->m_Values[key] = std::make_unique<BlackboardBoolValue>();
		auto bValue = (BlackboardBoolValue*)(m_Blackboard->m_Values[key].get());
		bValue->Value = value;
		
	}

	void BlackboardComponent::SetValueAsInt(const std::string& key, int value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		ClearValue(key);

		m_Blackboard->m_Values[key] = std::make_unique<BlackboardIntValue>();
		auto bValue = (BlackboardIntValue*)(m_Blackboard->m_Values[key].get());
		bValue->Value = value;
	}

	void BlackboardComponent::SetValueAsFloat(const std::string& key, float value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		ClearValue(key);

		

		m_Blackboard->m_Values[key] = std::make_unique<BlackboardFloatValue>();
		auto bValue = (BlackboardFloatValue*)(m_Blackboard->m_Values[key].get());
		bValue->Value = value;
	}

	void BlackboardComponent::SetValueAsFloat3(const std::string& key, const DirectX::XMFLOAT3& value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		ClearValue(key);

		m_Blackboard->m_Values[key] = std::make_unique<BlackboardFloat3Value>();
		auto bValue = (BlackboardFloat3Value*)(m_Blackboard->m_Values[key].get());
		bValue->Value = value;
	}

	void BlackboardComponent::SetValueAsFloat4(const std::string& key, const DirectX::XMFLOAT4& value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		ClearValue(key);

		m_Blackboard->m_Values[key] = std::make_unique<BlackboardFloat4Value>();
		auto bValue = (BlackboardFloat4Value*)(m_Blackboard->m_Values[key].get());
		bValue->Value = value;
	}

	void BlackboardComponent::SetValueAsString(const std::string& key, const std::string& value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		ClearValue(key);

		m_Blackboard->m_Values[key] = std::make_unique<BlackboardStringValue>();
		auto bValue = (BlackboardStringValue*)(m_Blackboard->m_Values[key].get());
		bValue->Value = value;
	}

	void BlackboardComponent::SetValueAsObject(const std::string& key, Weak<Object> value)
	{
		if (!m_Blackboard)
		{
			return;
		}

		ClearValue(key);

		

		m_Blackboard->m_Values[key] = std::make_unique<BlackboardObjectValue>();
		auto bValue = (BlackboardObjectValue*)(m_Blackboard->m_Values[key].get());
		bValue->Value = value;
	}

	void BlackboardComponent::ClearValue(const std::string& key)
	{
		auto it = m_Blackboard->m_Values.find(key);
		if(it != m_Blackboard->m_Values.end())
		{
			m_Blackboard->m_Values.erase(it);
		}
	}

	bool BlackboardComponent::HasValue(const std::string& key)
	{
		return m_Blackboard->m_Values.count(key) != 0;
	}

	uint64_t BlackboardComponent::GetValueID(const std::string& key) const
	{
		if (!m_Blackboard)
		{
			return 0;
		}

		auto it = m_Blackboard->m_Values.find(key);
		if (it != m_Blackboard->m_Values.end() && it->second)
		{
			return (it->second)->BlackboardID;
		}

		return 0;
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
			return static_cast<BlackboardBoolValue*>(it->second.get())->Value;
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
			return static_cast<BlackboardIntValue*>(it->second.get())->Value;
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
			return static_cast<BlackboardFloatValue*>(it->second.get())->Value;
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
			return static_cast<BlackboardFloat3Value*>(it->second.get())->Value;
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
			return static_cast<BlackboardFloat4Value*>(it->second.get())->Value;
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
			return static_cast<BlackboardStringValue*>(it->second.get())->Value;
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
			return static_cast<BlackboardObjectValue*>(it->second.get())->Value;
		}
		
		return {};
	}
}
