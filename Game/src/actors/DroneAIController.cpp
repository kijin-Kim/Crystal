#include "DroneAIController.h"

#include "CustomBTTask.h"
#include "Crystal/GamePlay/AI/BehaviorTree.h"
#include "Crystal/GamePlay/AI/Decorator.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/World/Level.h"

void DroneAIController::Initialize()
{
	Crystal::AIController::Initialize();

	m_AIPerceptionComponent->SetIsHearingEnabled(false);
	m_AIPerceptionComponent->SetHearingRange(4000.0f);

	m_AIPerceptionComponent->SetIsSightEnabled(true);
	m_AIPerceptionComponent->SetSightRange(4000.0f);
	m_AIPerceptionComponent->SetSightWidth(1500.0f * 2.0f);
	m_AIPerceptionComponent->SetSightHeight(900.0f * 2.5f);
	m_AIPerceptionComponent->IgnoreVisibilityActorClassOf("DroneLaser");
	m_AIPerceptionComponent->IgnoreVisibilityActorClassOf("Laser");
}

void DroneAIController::Begin()
{
	Crystal::AIController::Begin();

	

	auto behaviorTree = Crystal::CreateObject<Crystal::BehaviorTree>();
	auto rootNode = behaviorTree->GetRootNode();
	auto selectorNode = Crystal::CreateObject<Crystal::BTSelectorNode>("Selector");
	rootNode->AddChildNode(selectorNode);


	auto level = GetLevel().lock();
	if(level->GetObjectName() == "KrakenLevel")
	{

		m_AIPerceptionComponent->SetSightRange(10000.0f);
		m_AIPerceptionComponent->SetSightWidth(3000.0f * 2.0f);

		const float maxAcceleration = 50000000.0f;
		{
			auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceFire");
			auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
			blackboardBasedDecorator->BlackboardKey = "PlayerPawnObject";
			blackboardBasedDecorator->bIsSet = true;
			blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
			blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
			sequenceNode->AddDecorator(blackboardBasedDecorator);


			auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceActor>("TaskFaceLocation");
			faceLocationNode->TargetActorKey = "PlayerPawnObject";
			faceLocationNode->TargetAngleTolerance = 5.0f;
			sequenceNode->AddChildNode(faceLocationNode);

			auto fireNode = Crystal::CreateObject<BTTaskNodeDroneFire>("TaskNodeFire");
			sequenceNode->AddChildNode(fireNode);

			auto waitNode = Crystal::CreateObject<Crystal::BTTaskNodeWait>("TaskNodeWait");
			waitNode->WaitTime = m_FireInterval;
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
			blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
			sequenceNode->AddDecorator(blackboardBasedDecorator);

			auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
			faceLocationNode->TargetLocationKey = "LastSeenLocation";
			faceLocationNode->TargetAngleTolerance = 10.0f;
			sequenceNode->AddChildNode(faceLocationNode);

			auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
			moveToLocationNode->TargetLocationKey = "LastSeenLocation";
			moveToLocationNode->AcceptableRadius = 60.0f * 4.0f;
			moveToLocationNode->MaxAcceleration = maxAcceleration;
			sequenceNode->AddChildNode(moveToLocationNode);

			auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
			clearValueNode->BlackboardKey = "LastSeenLocation";
			sequenceNode->AddChildNode(clearValueNode);

			selectorNode->AddChildNode(sequenceNode);
		}

		// GoToRandom
		{
			auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceGoToRandom");
			auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
			blackboardBasedDecorator->BlackboardKey = "RandomPositionFromCenter";
			blackboardBasedDecorator->bIsSet = true;
			blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
			blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
			sequenceNode->AddDecorator(blackboardBasedDecorator);

			auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
			faceLocationNode->TargetLocationKey = "RandomPositionFromCenter";
			faceLocationNode->TargetAngleTolerance = 10.0f;
			sequenceNode->AddChildNode(faceLocationNode);

			auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
			moveToLocationNode->TargetLocationKey = "RandomPositionFromCenter";
			moveToLocationNode->AcceptableRadius = 60.0f * 4.0f;
			moveToLocationNode->MaxAcceleration = maxAcceleration;
			sequenceNode->AddChildNode(moveToLocationNode);

			auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
			clearValueNode->BlackboardKey = "RandomPositionFromCenter";
			sequenceNode->AddChildNode(clearValueNode);

			selectorNode->AddChildNode(sequenceNode);
		}


		// MoveToRandomPositionInCenter
		{
			auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToRandomPositionInSphere");

			auto setRandomPositionNode = Crystal::CreateObject<BTTaskNodeSetRandomPositionInSphereFromPosition>("TaskSetRandomPositionInSphere");
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
				// CS_DEBUG_INFO("Find Sight");

				auto instigator = sightStimulus.Instigator.lock();
				if (!instigator)
				{
					return;
				}

				if (sightStimulus.bIsSensed)
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
						m_BlackboardComponent->SetValueAsFloat3("LastSeenLocation", playerPawn->GetPosition());
					}
				}


			});


		m_AIPerceptionComponent->BindOnHearingUpdatedEvent([this](const Crystal::NoiseStimulus& noiseStimulus)
			{
				// CS_DEBUG_INFO("Find Noise");

				auto instigator = noiseStimulus.Instigator.lock();
				if (!instigator)
				{
					return;
				}

				if (noiseStimulus.bIsSensed)
				{
					if (instigator->StaticType() == "MyPlayerPawn")
					{
						m_BlackboardComponent->SetValueAsFloat3("LastSeenLocation", noiseStimulus.Position);
					}
				}

			});


		auto level = GetLevel().lock();
		m_BlackboardComponent->SetValueAsFloat3("LastSeenLocation", Crystal::Vector3::RandomPositionInSphere(Crystal::Cast<Actor>(level->GetPlayerPawn())->GetPosition(), 1000.0f));
	}
	else
	{
		const float maxAcceleration = 20000000.0f;
		{
			auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceFire");
			auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
			blackboardBasedDecorator->BlackboardKey = "PlayerPawnObject";
			blackboardBasedDecorator->bIsSet = true;
			blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
			blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
			sequenceNode->AddDecorator(blackboardBasedDecorator);


			auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceActor>("TaskFaceLocation");
			faceLocationNode->TargetActorKey = "PlayerPawnObject";
			faceLocationNode->TargetAngleTolerance = 5.0f;
			sequenceNode->AddChildNode(faceLocationNode);

			auto fireNode = Crystal::CreateObject<BTTaskNodeDroneFire>("TaskNodeFire");
			sequenceNode->AddChildNode(fireNode);

			auto waitNode = Crystal::CreateObject<Crystal::BTTaskNodeWait>("TaskNodeWait");
			waitNode->WaitTime = m_FireInterval;
			sequenceNode->AddChildNode(waitNode);

			selectorNode->AddChildNode(sequenceNode);
		}

		{
			auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceFire");
			auto blackboardBasedDecorator = Crystal::CreateObject<Crystal::BlackboardBasedDecorator>();
			blackboardBasedDecorator->BlackboardKey = "AttackTarget";
			blackboardBasedDecorator->bIsSet = true;
			blackboardBasedDecorator->AbortType = Crystal::EDecoratorAbortType::DAT_LowerPriority;
			blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
			sequenceNode->AddDecorator(blackboardBasedDecorator);


			auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceActor>("TaskFaceLocation");
			faceLocationNode->TargetActorKey = "AttackTarget";
			faceLocationNode->TargetAngleTolerance = 5.0f;
			sequenceNode->AddChildNode(faceLocationNode);

			auto fireNode = Crystal::CreateObject<BTTaskNodeDroneFire>("TaskNodeFire");
			sequenceNode->AddChildNode(fireNode);

			auto waitNode = Crystal::CreateObject<Crystal::BTTaskNodeWait>("TaskNodeWait");
			waitNode->WaitTime = m_FireInterval;
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
			blackboardBasedDecorator->AbortCondition = Crystal::EAbortCondition::AC_OnResultChange;
			sequenceNode->AddDecorator(blackboardBasedDecorator);

			auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
			faceLocationNode->TargetLocationKey = "LastSeenLocation";
			faceLocationNode->TargetAngleTolerance = 10.0f;
			sequenceNode->AddChildNode(faceLocationNode);

			auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
			moveToLocationNode->TargetLocationKey = "LastSeenLocation";
			moveToLocationNode->AcceptableRadius = 60.0f * 4.0f;
			moveToLocationNode->MaxAcceleration = maxAcceleration;
			sequenceNode->AddChildNode(moveToLocationNode);

			auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
			clearValueNode->BlackboardKey = "LastSeenLocation";
			sequenceNode->AddChildNode(clearValueNode);

			selectorNode->AddChildNode(sequenceNode);
		}

		// MoveToCruiser
		{
			auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToLastSeen");

			auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceActor>("TaskFaceLocation");
			faceLocationNode->TargetActorKey = "Target";
			faceLocationNode->TargetAngleTolerance = 10.0f;
			sequenceNode->AddChildNode(faceLocationNode);

			auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToActor>("TaskMoveToLocation");
			moveToLocationNode->TargetActorKey = "Target";
			moveToLocationNode->AcceptableRadius = 5000.0f;
			moveToLocationNode->MaxAcceleration = maxAcceleration;
			sequenceNode->AddChildNode(moveToLocationNode);

			auto clearValueNode = Crystal::CreateObject<Crystal::BTTaskNodeClearBlackboardValue>("TaskClearBlackboardValue");
			clearValueNode->BlackboardKey = "Target";
			sequenceNode->AddChildNode(clearValueNode);

			selectorNode->AddChildNode(sequenceNode);
		}


		SetBehaviorTree(behaviorTree);





		m_AIPerceptionComponent->BindOnSightUpdatedEvent([this](const Crystal::SightStimulus& sightStimulus)
			{
				// CS_DEBUG_INFO("Find Sight");

				auto instigator = sightStimulus.Instigator.lock();
				if (!instigator)
				{
					return;
				}

				if (sightStimulus.bIsSensed)
				{
					if (instigator->StaticType() == "Cruiser" || instigator->StaticType() == "DummySpaceStation")
					{
						m_BlackboardComponent->SetValueAsObject("AttackTarget", sightStimulus.Instigator);
						m_BlackboardComponent->ClearValue("LastSeenLocation");
					}
				}
				else
				{
					if (instigator->StaticType() == "Cruiser" || instigator->StaticType() == "DummySpaceStation")
					{
						auto playerPawn = Crystal::Cast<Actor>(m_BlackboardComponent->GetValueAsObject("AttackTarget"));
						m_BlackboardComponent->ClearValue("AttackTarget");
						m_BlackboardComponent->SetValueAsFloat3("LastSeenLocation", playerPawn->GetPosition());
					}
					
				}

				if (sightStimulus.bIsSensed)
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
						m_BlackboardComponent->SetValueAsFloat3("LastSeenLocation", playerPawn->GetPosition());
					}
				}


			});

	}

	

}

void DroneAIController::Update(float deltaTime)
{
	AIController::Update(deltaTime);

	auto level = GetLevel().lock();
	if(level)
	{
		if(level->GetObjectName() == "EscortLevel")
		{
			auto cruiser = level->GetActorByClass("Cruiser").lock();
			if(cruiser)
			{
				GetBlackboardComponent()->SetValueAsObject("Target", cruiser);
			}
		}else if(level->GetObjectName() == "AttackLevel")
		{
			auto spaceStation = level->GetActorByClass("DummySpaceStation").lock();
			if (spaceStation)
			{
				GetBlackboardComponent()->SetValueAsObject("Target", spaceStation);
			}
		}
		
	}

}
