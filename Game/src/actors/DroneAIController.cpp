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
		sequenceNode->AddDecorator(blackboardBasedDecorator);


		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceActor>("TaskFaceLocation");
		faceLocationNode->TargetActorKey = "PlayerPawnObject";
		faceLocationNode->TargetAngleTolerance = 5.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		auto fireNode = Crystal::CreateObject<BTTaskNodeDroneFire>("TaskNodeFire");
		sequenceNode->AddChildNode(fireNode);

		auto waitNode = Crystal::CreateObject<Crystal::BTTaskNodeWait>("TaskNodeWait");
		waitNode->WaitTime = 1.0f;
		sequenceNode->AddChildNode(waitNode);

		selectorNode->AddChildNode(sequenceNode);
	}





	//// MoveToRandomPositionInCenter
	//{
	//	auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceMoveToRandomPositionInSphere");

	//	auto setRandomPositionNode = Crystal::CreateObject<BTTaskNodeSetRandomPositionInSphere>("TaskSetRandomPositionInSphere");
	//	setRandomPositionNode->Center = Crystal::Vector3::Zero;
	//	setRandomPositionNode->Radius = 2000.0f;
	//	setRandomPositionNode->RandomPositionKey = "RandomPositionInSphere";
	//	sequenceNode->AddChildNode(setRandomPositionNode);

	//	auto moveToLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeMoveToLocation>("TaskMoveToLocation");
	//	moveToLocationNode->TargetLocationKey = "RandomPositionInSphere";
	//	moveToLocationNode->AcceptableRadius = 100.0f;
	//	moveToLocationNode->MaxAcceleration = 1200000.0f;
	//	sequenceNode->AddChildNode(moveToLocationNode);

	//	auto waitNode = Crystal::CreateObject<Crystal::BTTaskNodeWait>("TaskNodeWait");
	//	waitNode->WaitTime = 5.0f;
	//	sequenceNode->AddChildNode(waitNode);

	//	selectorNode->AddChildNode(sequenceNode);
	//}

	SetBehaviorTree(behaviorTree);




	m_AIPerceptionComponent->BindOnHearingUpdatedEvent([this](const Crystal::NoiseStimulus& noiseStimulus)
		{
			CS_DEBUG_INFO("Find Noise");

			auto instigator = noiseStimulus.Instigator.lock();
			if (!instigator)
			{
				return;
			}

			if (noiseStimulus.bIsSensed)
			{
				if (instigator->StaticType() == "MyPlayerPawn")
				{
					m_BlackboardComponent->SetValueAsObject("PlayerPawnObject", noiseStimulus.Instigator);
				}
			}
			else
			{
				if (instigator->StaticType() == "MyPlayerPawn")
				{
					m_BlackboardComponent->ClearValue("PlayerPawnObject");
				}
			}

		});

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
				}
			}
			else
			{
				if (instigator->StaticType() == "MyPlayerPawn")
				{
					m_BlackboardComponent->ClearValue("PlayerPawnObject");
				}
			}
				
			
		});


}
