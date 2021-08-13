#include "SpaceWhaleAIController.h"

#include "CustomBTTask.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/Types.h"
#include "Crystal/GamePlay/AI/BehaviorTree.h"
#include "Crystal/GamePlay/AI/Blackboard.h"
#include "Crystal/GamePlay/AI/Decorator.h"

void SpaceWhaleAIController::Initialize()
{
	Crystal::AIController::Initialize();

	m_AIPerceptionComponent->SetIsHearingEnabled(true);
	m_AIPerceptionComponent->SetHearingRange(4000.0f);

	m_AIPerceptionComponent->SetIsSightEnabled(true);
	m_AIPerceptionComponent->SetSightRange(5000.0f);
	m_AIPerceptionComponent->SetSightWidth(3000.0f);
	m_AIPerceptionComponent->SetSightHeight(3000.0f);
}

void SpaceWhaleAIController::Begin()
{
	Crystal::AIController::Begin();

	auto behaviorTree = Crystal::CreateObject<Crystal::BehaviorTree>();
	auto rootNode = behaviorTree->GetRootNode();
	auto selectorNode = Crystal::CreateObject<Crystal::BTSelectorNode>("Selector");
	rootNode->AddChildNode(selectorNode);


	// MoveToLastSeen
	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToLastSeen");
		auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
		blackboardBasedDecorator->BlackboardKey = "PlayerLocation";
		blackboardBasedDecorator->bIsSet = true;
		blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_Both;
		blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnValueChange;
		sequenceNode->AddDecorator(blackboardBasedDecorator);

		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
		faceLocationNode->TargetLocationKey = "PlayerLocation";
		faceLocationNode->TargetAngleTolerance = 10.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
		moveToLocationNode->TargetLocationKey = "PlayerLocation";
		moveToLocationNode->AcceptableRadius = 60.0f;
		moveToLocationNode->MaxAcceleration = 30000000.0f;
		sequenceNode->AddChildNode(moveToLocationNode);

		auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
		clearValueNode->BlackboardKey = "PlayerLocation";
		sequenceNode->AddChildNode(clearValueNode);


		selectorNode->AddChildNode(sequenceNode);
	}

	// GoToRandom
	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceGoToRandom");
		auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
		blackboardBasedDecorator->BlackboardKey = "RandomPositionInSphere";
		blackboardBasedDecorator->bIsSet = true;
		blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
		blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
		sequenceNode->AddDecorator(blackboardBasedDecorator);

		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
		faceLocationNode->TargetLocationKey = "RandomPositionInSphere";
		faceLocationNode->TargetAngleTolerance = 10.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
		moveToLocationNode->TargetLocationKey = "RandomPositionInSphere";
		moveToLocationNode->AcceptableRadius = 60.0f;
		moveToLocationNode->MaxAcceleration = 30000000.0f;
		sequenceNode->AddChildNode(moveToLocationNode);

		auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
		clearValueNode->BlackboardKey = "RandomPositionInSphere";
		sequenceNode->AddChildNode(clearValueNode);

		selectorNode->AddChildNode(sequenceNode);
	}


	// Keep Orbit
	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceOrbit");

		auto orbitNode = Crystal::CreateObject<BTTaskNodeOrbit>("TaskOrbit");
		orbitNode->MaxAcceleration = 30000000.0f;
		sequenceNode->AddChildNode(orbitNode);

		selectorNode->AddChildNode(sequenceNode);
	}

	SetBehaviorTree(behaviorTree);


	m_AIPerceptionComponent->BindOnHearingUpdatedEvent([this](const Crystal::NoiseStimulus& noiseStimulus)
	{
		auto instigator = noiseStimulus.Instigator.lock();
		if (!instigator)
		{
			return;
		}

		if (noiseStimulus.bIsSensed)
		{
			if (instigator->StaticType() == "MyPlayerPawn")
			{
				m_BlackboardComponent->SetValueAsFloat3("PlayerLocation", noiseStimulus.Position);
			}
		}
	});

	m_AIPerceptionComponent->BindOnSightUpdatedEvent([this](const Crystal::SightStimulus& sightStimulus)
	{
		auto instigator = sightStimulus.Instigator.lock();
		if (!instigator)
		{
			return;
		}

		if (instigator->StaticType() == "MyPlayerPawn")
		{
			if (sightStimulus.bIsSensed)
			{
				m_BlackboardComponent->SetValueAsFloat3("PlayerLocation", sightStimulus.Position);
			}
			else
			{
				//m_BlackboardComponent->ClearValue("PlayerLocation");
			}
		}
	});
}
