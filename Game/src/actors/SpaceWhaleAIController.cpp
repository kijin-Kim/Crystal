#include "SpaceWhaleAIController.h"

#include "Crystal/GamePlay/Components/AIComponent.h"

void SpaceWhaleAIController::Initialize()
{
	Crystal::AIController::Initialize();

	m_AIPerceptionComponent->SetIsHearingEnabled(true);
	m_AIPerceptionComponent->SetHearingRange(1000.0f);

	m_AIPerceptionComponent->SetIsSightEnabled(true);
	m_AIPerceptionComponent->SetSightRange(1000.0f);
	m_AIPerceptionComponent->SetSightWidth(1920.0f);
	m_AIPerceptionComponent->SetSightHeight(1080.0f);
}
