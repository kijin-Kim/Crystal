#include "SpaceWhaleAIController.h"

#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/Types.h"
#include "Crystal/GamePlay/AI/Blackboard.h"

void SpaceWhaleAIController::Initialize()
{
	Crystal::AIController::Initialize();

	m_AIPerceptionComponent->SetIsHearingEnabled(true);
	m_AIPerceptionComponent->SetHearingRange(1000.0f);

	m_AIPerceptionComponent->SetIsSightEnabled(true);
	m_AIPerceptionComponent->SetSightRange(5000.0f);
	m_AIPerceptionComponent->SetSightWidth(1366.0f);
	m_AIPerceptionComponent->SetSightHeight(720.0f);
}

void SpaceWhaleAIController::Begin()
{
	AIController::Begin();
	m_AIPerceptionComponent->BindOnHearingUpdatedEvent([this](const Crystal::NoiseStimulus& noiseStimulus)
	{
		if(!noiseStimulus.bIsSensed)
		{
			return;
		}
		
		CS_DEBUG_INFO("Find Noise");
	});

	m_AIPerceptionComponent->BindOnSightUpdatedEvent([this](const Crystal::SightStimulus& sightStimulus)
	{
		if(!sightStimulus.bIsSensed)
		{
			return;
		}

		CS_DEBUG_INFO("Find Sight");

		auto instigator = sightStimulus.Instigator.lock();
		if (!instigator)
		{
			return;
		}

		if (instigator->StaticType() == "MyPlayerPawn")
		{
			m_BlackboardComponent->SetValueAsFloat3("TargetLocation", sightStimulus.Position);
		}
	});
}
