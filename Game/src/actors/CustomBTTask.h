#pragma once
#include "Crystal/GamePlay/AI/BehaviorTree.h"


class BTTaskNodeSetTargetDirection : public Crystal::BTTaskNode
{
public:
	BTTaskNodeSetTargetDirection() = default;
	~BTTaskNodeSetTargetDirection() override = default;


	void Execute(float deltaTime) override
	{
		bool result = ExecuteDecorators();
		if (!result)
		{
			FinishExecute(false);
			return;
		}

		auto possessedPawn = GetPossessedPawn().lock();
		if (!possessedPawn)
		{
			FinishExecute(false);
			return;
		}

		possessedPawn->RotateYaw(0.1f);

	
		auto blackBoard = GetBlackboardComponent().lock();
		blackBoard->SetValueAsFloat3(TargetDirectionKey, possessedPawn->GetForwardVector());

		FinishExecute(true);
	}

	STATIC_TYPE_IMPLE(BTTaskNodeSetTargetDirection)

public:
	std::string TargetDirectionKey;
};


class BTTaskNodeMoveTowardDirection : public Crystal::BTTaskNode
{
public:
	BTTaskNodeMoveTowardDirection() = default;
	~BTTaskNodeMoveTowardDirection() override = default;


	void Execute(float deltaTime) override
	{
		bool result = ExecuteDecorators();
		if (!result)
		{
			FinishExecute(false);
			return;
		}


		if (TargetDirectionKey.empty())
		{
			FinishExecute(false);
			return;
		}

		auto possessedPawn = GetPossessedPawn().lock();
		if (!possessedPawn)
		{
			FinishExecute(false);
			return;
		}

		auto blackBoard = GetBlackboardComponent().lock();
		auto targetDirection = blackBoard->GetValueAsFloat3(TargetDirectionKey);

		possessedPawn->AddForce(Crystal::Vector3::Multiply(targetDirection, MaxAcceleration));

		FinishExecute(true);
	}

	STATIC_TYPE_IMPLE(BTTaskNodeMoveTowardDirection)

public:
	std::string TargetDirectionKey;
	float MaxAcceleration = 0.0f;
};