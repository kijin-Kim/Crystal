#pragma once
#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {

	struct IBlackboardValue
	{
	};

	struct BlackBoardBoolValue : public IBlackboardValue
	{
		bool Value = false;
	};

	struct BlackBoardIntValue : public IBlackboardValue
	{
		int Value = 0;
	};

	struct BlackBoardFloatValue : public IBlackboardValue
	{
		float Value = 0.0f;
	};

	struct BlackBoardFloat3Value : public IBlackboardValue
	{
		DirectX::XMFLOAT3 Value = Vector3::Zero;
	};

	struct BlackBoardFloat4Value : public IBlackboardValue
	{
		DirectX::XMFLOAT4 Value = Vector4::Zero;
	};

	struct BlackBoardStringValue : public IBlackboardValue
	{
		std::string Value = {};
	};

	struct BlackBoardObjectValue : public IBlackboardValue
	{
		Weak<Object> Value = {};
	};

	

	class Blackboard : public Object
	{
		friend class BlackboardComponent;
	public:
		Blackboard() = default;
		~Blackboard() override = default;

	
		STATIC_TYPE_IMPLE(Blackboard)

	private:
		std::unordered_map<std::string, IBlackboardValue*> m_Values;
	};

	class BlackboardComponent : public Component
	{
	public:
		BlackboardComponent() = default;
		~BlackboardComponent() override = default;

		void Initialize() override;

		void SetValueAsBool(const std::string& key, bool value);
		void SetValueAsInt(const std::string& key, int value);
		void SetValueAsFloat(const std::string& key, float value);
		void SetValueAsFloat3(const std::string& key, const DirectX::XMFLOAT3& value);
		void SetValueAsFloat4(const std::string& key, const DirectX::XMFLOAT4& value);
		void SetValueAsString(const std::string& key, const std::string& value);
		void SetValueAsObject(const std::string& key, Weak<Object> value);

		void ClearValue(const std::string& key);

		
		bool HasValue(const std::string& key);
		


		bool GetValueAsBool(const std::string& key) const;
		int GetValueAsInt(const std::string& key) const;
		float GetValueAsFloat(const std::string& key) const;
		const DirectX::XMFLOAT3& GetValueAsFloat3(const std::string& key) const;
		const DirectX::XMFLOAT4& GetValueAsFloat4(const std::string& key) const;
		const std::string& GetValueAsString(const std::string& key) const;
		Weak<Object> GetValueAsObject(const std::string& key) const;

		
		
		STATIC_TYPE_IMPLE(BlackboardComponent)

	private:
		Shared<Blackboard> m_Blackboard = nullptr;
	};
	
}
