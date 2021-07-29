#include "cspch.h"
#include "AIController.h"

#include "Crystal/GamePlay/AI/BehaviorTree.h"
#include "Crystal/GamePlay/AI/Blackboard.h"
//#include "Crystal/GamePlay/Components/AIComponent.h"

namespace Crystal {


	void AIController::Initialize()
	{
		Controller::Initialize();

		m_BehaviorTreeComponent = CreateComponent<BehaviorTreeComponent>("BehaviorTreeComponent");
		m_BlackboardComponent = CreateComponent<BlackboardComponent>("BlackboardComponent");
		//m_AIPerceptionComponent = CreateComponent<AIPerceptionComponent>("AIPerceptionComponent");
	}

	void AIController::SetBehaviorTree(Shared<BehaviorTree> behaviorTree)
	{
		m_BehaviorTreeComponent->SetBehaviorTree(behaviorTree);
		m_BehaviorTreeComponent->SetOuter(weak_from_this());
	}

	Shared<BlackboardComponent> AIController::GetBlackboardComponent() const
	{
		return m_BlackboardComponent;
		
	}


	void AIController::MoveToLocation(const DirectX::XMFLOAT3& location, float maxAccel, float acceptableRadius)
	{
		m_MoveToLocationRequest.TargetLocation = location;
		m_MoveToLocationRequest.bShouldMoveToTargetLocation = true;
		m_MoveToLocationRequest.MaxAcceleration = maxAccel;
		m_MoveToLocationRequest.AcceptableRadius = acceptableRadius;
	}

	void AIController::ProcessMoveToLocationRequest()
	{
		auto pawn = m_PossessedPawn.lock();
		if (!pawn)
		{
			return;
		}

		auto mainComponent = pawn->GetMainComponent().lock();
		if (!mainComponent)
		{
			return;
		}

		if (!mainComponent->HasFiniteMass())
		{
			return;
		}

		if (!m_MoveToLocationRequest.bShouldMoveToTargetLocation)
		{
			return;
		}


		auto distance = Vector3::Subtract(m_MoveToLocationRequest.TargetLocation, mainComponent->GetWorldPosition());
		float lengthSq = Vector3::LengthSquared(distance);
		float acceptableRadiusSq = m_MoveToLocationRequest.AcceptableRadius * m_MoveToLocationRequest.AcceptableRadius;

		if (lengthSq <= acceptableRadiusSq)
		{
			m_MoveToLocationRequest = {};
			return;
		}

		auto direction = Vector3::Normalize(distance);

		mainComponent->AddForce(Vector3::Multiply(direction, m_MoveToLocationRequest.MaxAcceleration));
	}

	void AIController::Update(float deltaTime)
	{
		Controller::Update(deltaTime);

		ProcessMoveToLocationRequest();
	}
}
