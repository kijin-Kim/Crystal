#pragma once
#include "Inventory.h"
#include "Crystal/GamePlay/Objects/Object.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/Math/Math.h"

namespace Crystal {
	class Level;
}

struct QuestReward
{
	uint32_t Gold;
	std::vector<EItemType> ItemType;
	std::vector<uint32_t> ItemCount;
};


class Quest : public Crystal::Object
{
public:
	enum class EQuestType
	{
		QT_None,
		QT_Location,
		QT_Destroy
	};

public:
	Quest() = default;
	~Quest() override = default;

	void SetQuestText(const std::string& questText) { m_QuestText = questText; }
	const std::string& GetQuestText() { return m_QuestText; }

	void SetQuestType(EQuestType type) { m_QuestType = type; }
	EQuestType GetQuestType() const { return m_QuestType; }

	void SetQuestReward(const QuestReward& reward) { m_Reward = reward; }

	void SetQuestTargetLocation(const DirectX::XMFLOAT3& location) { m_TargetLocation = location; }
	void SetQuestTargetActor(Crystal::Weak<Crystal::Actor> actor) { m_TargetActor = actor; }
	void SetQuestTargetType(const std::string& type) { m_TargetTypes.push_back(type); }
	void SetQuestTargetTypeCount(uint32_t targetCount) { m_TargetCount = targetCount; }


	void OnActorDestroyed(const std::string& type)
	{
		if(m_QuestType != EQuestType::QT_Destroy)
		{
			return;
		}

		auto it = std::find(m_TargetTypes.begin(), m_TargetTypes.end(), type);
		if(it == m_TargetTypes.end())
		{
			return;
		}

		if(!m_bIsCompleted)
		{
			m_CurrentCount++;
			if (m_CurrentCount >= m_TargetCount)
			{
				m_bIsCompleted = true;
			}
		}
		
	}

	const DirectX::XMFLOAT3& GetQuestTargetLocation() const { return m_TargetLocation; }
	uint32_t GetQuestTargetTypeCount() const { return m_TargetCount; }
	uint32_t GetQuestTargetTypeCurrentCount() const { return m_CurrentCount; }

	bool GetIsCompleted() const { return m_bIsCompleted; }

	void Update(float deltaTime) override;

	const QuestReward& GetQuestReward() const { return m_Reward; }


	STATIC_TYPE_IMPLE(Quest)
private:
	std::string m_QuestText;
	EQuestType m_QuestType = EQuestType::QT_None;


	DirectX::XMFLOAT3 m_TargetLocation = Crystal::Vector3::Zero;
	std::vector<std::string> m_TargetTypes;
	uint32_t m_TargetCount = 0;
	uint32_t m_CurrentCount = 0;

	Crystal::Weak<Crystal::Actor> m_TargetActor;

	QuestReward m_Reward;

	bool m_bIsCompleted = false;
};
