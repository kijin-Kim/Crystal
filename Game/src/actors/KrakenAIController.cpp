#include "KrakenAIController.h"

#include "MyPlayerPawn.h"
#include "Crystal/GamePlay/AI/BehaviorTree.h"
#include "Crystal/GamePlay/Components/AIComponent.h"
#include "Crystal/GamePlay/World/Level.h"

void KrakenAIController::Initialize()
{
	AIController::Initialize();
}

void KrakenAIController::Begin()
{
	Crystal::AIController::Begin();

	auto behaviorTree = Crystal::CreateObject<Crystal::BehaviorTree>();
	auto rootNode = behaviorTree->GetRootNode();
	auto selectorNode = Crystal::CreateObject<Crystal::BTSelectorNode>("Selector");
	rootNode->AddChildNode(selectorNode);


	// MoveToLastSeen
	{
		auto sequenceNode = Crystal::CreateObject<Crystal::BTSequenceNode>("SequenceFaceLocation");

		auto faceLocationNode = Crystal::CreateObject<Crystal::BTTaskNodeFaceLocation>("TaskFaceLocation");
		faceLocationNode->TargetLocationKey = "PlayerLocation";
		faceLocationNode->TargetAngleTolerance = 0.0f;
		sequenceNode->AddChildNode(faceLocationNode);

		selectorNode->AddChildNode(sequenceNode);
	}

	SetBehaviorTree(behaviorTree);
}

void KrakenAIController::Update(float deltaTime)
{
	AIController::Update(deltaTime);

	auto level = Crystal::Cast<Crystal::Level>(GetLevel());
	if(level)
	{
		auto playerPawn = Crystal::Cast<MyPlayerPawn>(level->GetPlayerPawn());
		if(playerPawn)
		{
			auto blackboardComponent = GetBlackboardComponent();
			blackboardComponent->SetValueAsFloat3("PlayerLocation", playerPawn->GetPosition());
		}
	}
	
}


