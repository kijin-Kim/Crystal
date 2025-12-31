#include "Quest.h"
#include "Crystal/GamePlay/World/Level.h"


void Quest::Update(float deltaTime)
{
	Crystal::Object::Update(deltaTime);

	switch (m_QuestType)
	{
	case EQuestType::QT_None: break;
	case EQuestType::QT_Location:
		{
			auto targetActor = m_TargetActor.lock();
			if (targetActor)
			{
				auto lengthSquared = Crystal::Vector3::LengthSquared(Crystal::Vector3::Subtract(targetActor->GetPosition(), m_TargetLocation));
				if (lengthSquared <= 1000.0f * 1000.0f)
				{
					m_bIsCompleted = true;
				}
			}
		}

		break;
	case EQuestType::QT_Destroy:
		break;
	default: ;
	}
}


