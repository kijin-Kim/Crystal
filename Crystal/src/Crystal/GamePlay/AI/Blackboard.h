#pragma once
#include <unordered_map>
#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/GamePlay/Objects/Object.h"
#include "Crystal/Types.h"

namespace Crystal {

	enum class BlackboardValueType
	{
		BVT_None,
		BVT_Bool,
		BVT_Int,
		BVT_Float,
		BVT_Float3,
		BVT_Float4,
		BVT_String,
		BVT_Object,
	};

	struct IBlackboardValue
	{
		uint64_t BlackboardID = IDCounter<IBlackboardValue>::Instance().GetNewID();
	};

	struct BlackboardBoolValue : public IBlackboardValue
	{
		bool Value = false;
	};

	struct BlackboardIntValue : public IBlackboardValue
	{
		int Value = 0;
	};

	struct BlackboardFloatValue : public IBlackboardValue
	{
		float Value = 0.0f;
	};

	struct BlackboardFloat3Value : public IBlackboardValue
	{

		DirectX::XMFLOAT3 Value = Vector3::Zero;
	};

	struct BlackboardFloat4Value : public IBlackboardValue
	{
		DirectX::XMFLOAT4 Value = Vector4::Zero;
	};

	struct BlackboardStringValue : public IBlackboardValue
	{
		std::string Value = {};
	};

	struct BlackboardObjectValue : public IBlackboardValue
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

	public:
		std::unordered_map<std::string, std::unique_ptr<IBlackboardValue>> m_Values;
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
		

		uint64_t GetValueID(const std::string& key) const;
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


