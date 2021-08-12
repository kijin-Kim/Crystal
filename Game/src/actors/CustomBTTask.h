#pragma once
#include "Drone.h"
#include "Crystal/GamePlay/AI/BehaviorTree.h"

class BTTaskNodeRotate : public Crystal::BTTaskNode
{
public:
	BTTaskNodeRotate() = default;
	~BTTaskNodeRotate() override = default;


	void Execute(float deltaTime) override
	{
		bool abortResult = CheckAbortDecorators();
		if (!abortResult)
		{
			Abort();
			return;
		}

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

		possessedPawn->RotateYaw(0.1f * deltaTime);

		FinishExecute(true);
	}

	STATIC_TYPE_IMPLE(BTTaskNodeRotate)
};


class BTTaskNodeSetRandomPositionInSphere : public Crystal::BTTaskNode
{
public:
	BTTaskNodeSetRandomPositionInSphere() = default;
	~BTTaskNodeSetRandomPositionInSphere() override = default;


	void Execute(float deltaTime) override
	{

		bool abortResult = CheckAbortDecorators();
		if (!abortResult)
		{
			Abort();
			return;
		}
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

		auto blackBoard = GetBlackboardComponent().lock();
		blackBoard->SetValueAsFloat3(RandomPositionKey, Crystal::Vector3::RandomPositionInSphere(possessedPawn->GetPosition(), Radius));

		FinishExecute(true);
	}

	STATIC_TYPE_IMPLE(BTTaskNodeSetRandomPositionInSphere)

public:
	std::string RandomPositionKey;
	float Radius;
};





class BTTaskNodeSetTargetDirection : public Crystal::BTTaskNode
{
public:
	BTTaskNodeSetTargetDirection() = default;
	~BTTaskNodeSetTargetDirection() override = default;


	void Execute(float deltaTime) override
	{
		bool abortResult = CheckAbortDecorators();
		if (!abortResult)
		{
			Abort();
			return;
		}

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

		auto blackBoard = GetBlackboardComponent().lock();
		blackBoard->SetValueAsFloat3(TargetDirectionKey, possessedPawn->GetForwardVector());

		FinishExecute(true);
	}

	STATIC_TYPE_IMPLE(BTTaskNodeSetTargetDirection)

public:
	std::string TargetDirectionKey;
};


class BTTaskNodeOrbit : public Crystal::BTTaskNode
{
public:
	BTTaskNodeOrbit() = default;
	~BTTaskNodeOrbit() override = default;


	void Execute(float deltaTime) override
	{
		bool abortResult = CheckAbortDecorators();
		if (!abortResult)
		{
			Abort();
			return;
		}

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

		possessedPawn->RotateYaw(10.0f * deltaTime);
		auto targetDirection = Crystal::Vector3::Normalize(possessedPawn->GetForwardVector());
		possessedPawn->AddForce(Crystal::Vector3::Multiply(targetDirection, MaxAcceleration));

//		FinishExecute(true);
	}

	STATIC_TYPE_IMPLE(BTTaskNodeOrbit)

public:
	float MaxAcceleration = 0.0f;
};


class BTTaskNodeDroneFire : public Crystal::BTTaskNode
{
public:
	BTTaskNodeDroneFire() = default;
	~BTTaskNodeDroneFire() override = default;


	void Execute(float deltaTime) override
	{
		bool abortResult = CheckAbortDecorators();
		if (!abortResult)
		{
			Abort();
			return;
		}

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

		if(possessedPawn->StaticType() != "Drone")
		{
			FinishExecute(false);
			return;
		}

		auto drone = Crystal::Cast<Drone>(possessedPawn);
		drone->OnFire();

		FinishExecute(true);
	}

	STATIC_TYPE_IMPLE(BTTaskNodeDroneFire)

public:
	std::string RandomPositionKey;
	DirectX::XMFLOAT3 Center;
	float Radius;
};