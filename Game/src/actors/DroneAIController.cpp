#include "DroneAIController.h"

#include "CustomBTTask.h"
#include "Crystal/GamePlay/AI/BehaviorTree.h"
#include "Crystal/GamePlay/AI/Decorator.h"
#include "Crystal/GamePlay/Components/AIComponent.h"

void DroneAIController::Initialize()
{
	Crystal::AIController::Initialize();

	m_AIPerceptionComponent->SetIsHearingEnabled(true);
	m_AIPerceptionComponent->SetHearingRange(400.0f);

	m_AIPerceptionComponent->SetIsSightEnabled(true);
	m_AIPerceptionComponent->SetSightRange(1500.0f);
	m_AIPerceptionComponent->SetSightWidth(1366.0f);
	m_AIPerceptionComponent->SetSightHeight(720.0f);
	m_AIPerceptionComponent->IgnoreVisibilityActorClassOf("DroneLaser");
}

void DroneAIController::Begin()
{
	Crystal::AIController::Begin();

	auto behaviorTree = Crystal::CreateObject<Crystal::BehaviorTree>();
	auto rootNode = behaviorTree->GetRootNode();
	auto selectorNode = Crystal::CreateObject<Crystal::BTSelectorNode>("Selector");
	rootNode->AddChildNode(selectorNode);


	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceFire");
		auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
		blackboardBasedDecorator->BlackboardKey = "PlayerPawnObject";
		blackboardBasedDecorator->bIsSet = true;
		blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
		sequenceNode->AddDecorator(blackboardBasedDecorator);


		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceActor>("TaskFaceLocation");
		faceLocationNode->TargetActorKey = "PlayerPawnObject";
		faceLocationNode->TargetAngleTolerance = 5.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto fireNode = Crystal::CreateObject<BTTaskNodeDroneFire>("TaskNodeFire");
		sequenceNode->AddChildNode(fireNode);

		auto waitNode = Crystal::CreateObject<Crystal::BTTaskNodeWait>("TaskNodeWait");
		waitNode->WaitTime = 0.4f;
		sequenceNode->AddChildNode(waitNode);

		selectorNode->AddChildNode(sequenceNode);
	}


	// MoveToLastSeen
	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToLastSeen");
		auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
		blackboardBasedDecorator->BlackboardKey = "LastSeenLocation";
		blackboardBasedDecorator->bIsSet = true;
		blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
		sequenceNode->AddDecorator(blackboardBasedDecorator);

		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
		faceLocationNode->TargetLocationKey = "LastSeenLocation";
		faceLocationNode->TargetAngleTolerance = 10.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
		moveToLocationNode->TargetLocationKey = "LastSeenLocation";
		moveToLocationNode->AcceptableRadius = 60.0f;
		moveToLocationNode->MaxAcceleration = 5000000.0f;
		sequenceNode->AddChildNode(moveToLocationNode);

		auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
		clearValueNode->BlackboardKey = "LastSeenLocation";
		sequenceNode->AddChildNode(clearValueNode);

		selectorNode->AddChildNode(sequenceNode);
	}


	// MoveToRandomPositionInCenter
	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToRandomPositionInSphere");

		auto setRandomPositionNode = Crystal::CreateObject<BTTaskNodeSetRandomPositionInSphere>("TaskSetRandomPositionInSphere");
		setRandomPositionNode->Radius = 2000.0f;
		setRandomPositionNode->RandomPositionKey = "RandomPositionInSphere";
		sequenceNode->AddChildNode(setRandomPositionNode);

		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
		faceLocationNode->TargetLocationKey = "RandomPositionInSphere";
		faceLocationNode->TargetAngleTolerance = 10.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
		moveToLocationNode->TargetLocationKey = "RandomPositionInSphere";
		moveToLocationNode->AcceptableRadius = 60.0f;
		moveToLocationNode->MaxAcceleration = 5000000.0f;
		sequenceNode->AddChildNode(moveToLocationNode);

		auto waitNode = Crystal::CreateObject<Crystal::BTTaskNodeWait>("TaskNodeWait");
		waitNode->WaitTime = 3.0f;
		sequenceNode->AddChildNode(waitNode);

		selectorNode->AddChildNode(sequenceNode);
	}

	SetBehaviorTree(behaviorTree);



	m_AIPerceptionComponent->BindOnSightUpdatedEvent([this](const Crystal::SightStimulus& sightStimulus)
		{
			CS_DEBUG_INFO("Find Sight");

			auto instigator = sightStimulus.Instigator.lock();
			if (!instigator)
			{
				return;
			}

			if(sightStimulus.bIsSensed)
			{
				if (instigator->StaticType() == "MyPlayerPawn")
				{
					m_BlackboardComponent->SetValueAsObject("PlayerPawnObject", sightStimulus.Instigator);
					m_BlackboardComponent->ClearValue("LastSeenLocation");
				}
			}
			else
			{
				if (instigator->StaticType() == "MyPlayerPawn")
				{
					auto playerPawn = Crystal::Cast<Actor>(m_BlackboardComponent->GetValueAsObject("PlayerPawnObject"));
					m_BlackboardComponent->ClearValue("PlayerPawnObject");
					m_BlackboardComponent->SetValueAsFloat3("LastSeenLocation",playerPawn->GetPosition());
					
				}
			}
				
			
		});


}
