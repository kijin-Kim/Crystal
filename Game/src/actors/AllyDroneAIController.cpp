#include "AllyDroneAIController.h"

#include "CustomBTTask.h"
#include "Crystal/GamePlay/AI/BehaviorTree.h"
#include "Crystal/GamePlay/AI/Decorator.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/World/Level.h"


void AllyDroneAIController::Initialize()
{
	AIController::Initialize();

	m_AIPerceptionComponent->SetIsHearingEnabled(false);
	m_AIPerceptionComponent->SetHearingRange(4000.0f);

	m_AIPerceptionComponent->SetIsSightEnabled(true);
	m_AIPerceptionComponent->SetSightRange(3000.0f);
	m_AIPerceptionComponent->SetSightWidth(1500.0f * 2.0f);
	m_AIPerceptionComponent->SetSightHeight(900.0f * 2.5f);
	m_AIPerceptionComponent->IgnoreVisibilityActorClassOf("DroneLaser");
	m_AIPerceptionComponent->IgnoreVisibilityActorClassOf("Laser");
}

void AllyDroneAIController::Begin()
{
	AIController::Begin();


	const float maxAcceleration = 8000000.0f;

	auto behaviorTree = Crystal::CreateObject<Crystal::BehaviorTree>();
	auto rootNode = behaviorTree->GetRootNode();
	auto selectorNode = Crystal::CreateObject<Crystal::BTSelectorNode>("Selector");
	rootNode->AddChildNode(selectorNode);


	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceFire");
		auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
		blackboardBasedDecorator->BlackboardKey = "TargetAsteroid";
		blackboardBasedDecorator->bIsSet = true;
		blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
		blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
		sequenceNode->AddDecorator(blackboardBasedDecorator);


		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceActor>("TaskFaceLocation");
		faceLocationNode->TargetActorKey = "TargetAsteroid";
		faceLocationNode->TargetAngleTolerance = 5.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto fireNode = Crystal::CreateObject<BTTaskNodeAllyDroneFire>("TaskNodeFire");
		sequenceNode->AddChildNode(fireNode);

		auto waitNode = Crystal::CreateObject<Crystal::BTTaskNodeWait>("TaskNodeWait");
		waitNode->WaitTime = m_FireInterval;
		sequenceNode->AddChildNode(waitNode);

		selectorNode->AddChildNode(sequenceNode);
	}


	// MoveToMine
	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToMine");
		auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
		blackboardBasedDecorator->BlackboardKey = "bShouldMoveToMine";
		blackboardBasedDecorator->bIsSet = true;
		blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
		blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
		sequenceNode->AddDecorator(blackboardBasedDecorator);

		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
		faceLocationNode->TargetLocationKey = "MineLocation";
		faceLocationNode->TargetAngleTolerance = 10.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
		moveToLocationNode->TargetLocationKey = "MineLocation";
		moveToLocationNode->AcceptableRadius = 60.0f * 4.0f;
		moveToLocationNode->MaxAcceleration = maxAcceleration;
		sequenceNode->AddChildNode(moveToLocationNode);

		auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
		clearValueNode->BlackboardKey = "bShouldMoveToMine";
		sequenceNode->AddChildNode(clearValueNode);

		selectorNode->AddChildNode(sequenceNode);
	}

	// MoveToRandomPositionInCenter
	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToRandomPositionInSphere");

		auto setRandomPositionNode = Crystal::CreateObject<BTTaskNodeSetRandomPositionInSphere>("TaskSetRandomPositionInSphere");
		setRandomPositionNode->Center = { 0.0f, 15000.0f, 15000.0f };
		setRandomPositionNode->Radius = 10000.0f;
		setRandomPositionNode->RandomPositionKey = "RandomPositionInSphere";
		sequenceNode->AddChildNode(setRandomPositionNode);

		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
		faceLocationNode->TargetLocationKey = "RandomPositionInSphere";
		faceLocationNode->TargetAngleTolerance = 2.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
		moveToLocationNode->TargetLocationKey = "RandomPositionInSphere";
		moveToLocationNode->AcceptableRadius = 60.0f * 4.0f;
		moveToLocationNode->MaxAcceleration = maxAcceleration;
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

			if (sightStimulus.bIsSensed)
			{
				auto staticType = instigator->StaticType();
				if (staticType == "GreenOreAsteroid" || staticType == "YellowOreAsteroid" || staticType == "BlueOreAsteroid")
				{
					if(!m_BlackboardComponent->HasValue("TargetAsteroid"))
					{
						m_BlackboardComponent->SetValueAsObject("TargetAsteroid", sightStimulus.Instigator);
					}
				}
			}
			else
			{
				auto staticType = instigator->StaticType();
				if (staticType == "GreenOreAsteroid" || staticType == "YellowOreAsteroid" || staticType == "BlueOreAsteroid")
				{
					m_BlackboardComponent->ClearValue("TargetAsteroid");
				}
			}

		});

}

void AllyDroneAIController::OnPosses(Crystal::Shared<Crystal::Pawn> pawn)
{
	AIController::OnPosses(pawn);

	if (GetLevel().lock()->GetObjectName() == "MiningLevel")
	{
		m_BlackboardComponent->SetValueAsFloat3("MineLocation", { pawn->GetPosition().x, 15000.0f, 15000.0f });
		m_BlackboardComponent->SetValueAsBool("bShouldMoveToMine", true);
	}

}
