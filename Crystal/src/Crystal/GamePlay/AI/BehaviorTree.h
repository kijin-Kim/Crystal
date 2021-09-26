#pragma once
#include "Blackboard.h"
#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/GamePlay/Controllers/AIController.h"
#include "Crystal/GamePlay/Objects/Object.h"


namespace Crystal {
	class Decorator;
	class BTCompositeNode;
	class AIController;
	class BehaviorTreeComponent;
	class BehaviorTree;
	class BlackboardComponent;


	class BTCompositeNode : public Object
	{
	public:
		BTCompositeNode() = default;
		~BTCompositeNode() override = default;

		void AddChildNode(Shared<BTCompositeNode> childNode)
		{
			m_ChildNodes.push_back(childNode);
			childNode->SetParentNode(Cast<BTCompositeNode>(weak_from_this()));
		}

		virtual void Execute(float deltaTime)
		{
			if(!m_ChildNodes.empty())
			{
				m_ChildNodes[0]->Execute(deltaTime);
			}
		}

		void ClearChildNodeExecutionFlags()
		{
			for(auto& childNode : m_ChildNodes)
			{
				childNode->ClearExecutionFlags();
				childNode->ClearChildNodeExecutionFlags();
			}
		}

		void Abort()
		{
			auto parentNode = Cast<BTCompositeNode>(GetParentNode());
			if(parentNode)
			{
				parentNode->Abort();
				if (parentNode == Cast<BTCompositeNode>(GetRootNode()))
				{
					FinishExecute(false);
					return;
				}

				if (parentNode->StaticType() == "BTSelectorNode")
				{
					FinishExecute(true);
				}
				else // BTSequenceNode
				{
					FinishExecute(false);
				}
			}
			else
			{
				ClearChildNodeExecutionFlags();
			}
		}


		void SetParentNode(Weak<BTCompositeNode> parentNode) { m_ParentNode = parentNode; }
		void AddDecorator(const Shared<Decorator>& decorator);
		void AddAbortDecorator(Weak<Decorator> decorator);
		void AddAbortDecoratorToChilds(Weak<Decorator> decorator);
		void PrepareAbortDecorators();

		Weak<BTCompositeNode> GetParentNode() const { return m_ParentNode; }
		Weak<BTCompositeNode> GetRootNode();
		Weak<BehaviorTree> GetBehaviorTree();
		Weak<BehaviorTreeComponent> GetBehaviorTreeComponent();
		Weak<AIController> GetAIController();
		Weak<Pawn> GetPossessedPawn();
		Weak<BlackboardComponent> GetBlackboardComponent();


		bool ExecuteDecorators() const;
		bool CheckAbortDecorators() const;

		void FinishExecute(bool bResult)
		{
			m_bIsExecutionFinished = true;
			m_bExecutionResult = bResult;
		}

		void ClearExecutionFlags()
		{
			m_bIsExecutionFinished = false;
			m_bExecutionResult = false;
		}

		bool GetExecutionResult() const
		{
			return m_bExecutionResult;
		}

		bool GetIsExecutionFinished() const
		{
			return m_bIsExecutionFinished;
		}

		bool GetHasDecorators() const
		{
			return !m_Decorators.empty();
		}


		const std::vector<Shared<Decorator>>& GetDecorators() const { return m_Decorators; }
		const std::vector<Shared<BTCompositeNode>>& GetChildNodes() const { return m_ChildNodes; }

		STATIC_TYPE_IMPLE(BTCompositeNode)

	protected:
		std::vector<Shared<BTCompositeNode>> m_ChildNodes;
		Weak<BTCompositeNode> m_ParentNode;
		std::vector<Shared<Decorator>> m_Decorators;
		std::vector<Weak<Decorator>> m_AbortDecorators;
		bool m_bExecutionResult;
		bool m_bIsExecutionFinished = false;
	};

	class BTSelectorNode : public BTCompositeNode
	{
	public:
		BTSelectorNode() = default;
		~BTSelectorNode() override = default;

		void Execute(float deltaTime) override
		{
			auto abortResult = CheckAbortDecorators();
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

			for (auto& childNode : m_ChildNodes)
			{
				if(childNode->GetIsExecutionFinished())
				{
					bool bExecutionResult = childNode->GetExecutionResult();
					if(bExecutionResult) // if child succeed break
					{
						ClearChildNodeExecutionFlags();
						FinishExecute(true);
						return;
					}
				}
				else
				{
					childNode->Execute(deltaTime);
					return;
				}
			}

			// if all node failed
			ClearChildNodeExecutionFlags();
			FinishExecute(false);
		}

		STATIC_TYPE_IMPLE(BTSelectorNode)
	};

	class BTSequenceNode : public BTCompositeNode
	{
	public:
		BTSequenceNode() = default;
		~BTSequenceNode() override = default;

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

			for (auto& childNode : m_ChildNodes)
			{
				if (childNode->GetIsExecutionFinished())
				{
					bool bExecutionResult = childNode->GetExecutionResult();
					if (!bExecutionResult)
					{
						ClearChildNodeExecutionFlags();
						FinishExecute(false);
						return;
					}
				}
				else
				{
					childNode->Execute(deltaTime);
					return;
				}
			}
			// if all node succeed
			ClearChildNodeExecutionFlags();
			FinishExecute(true);
		}

		STATIC_TYPE_IMPLE(BTSequenceNode)
	};

	class BTTaskNode : public BTCompositeNode
	{
	public:
		BTTaskNode() = default;
		~BTTaskNode() override = default;


		STATIC_TYPE_IMPLE(BTTaskNode)
	};

	class BTTaskNodeMoveToActor : public BTTaskNode
	{
	public:
		BTTaskNodeMoveToActor() = default;
		~BTTaskNodeMoveToActor() override = default;


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


			if (TargetActorKey.empty())
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
			auto targetActor = Cast<Actor>(blackBoard->GetValueAsObject(TargetActorKey));
			if(!targetActor)
			{
				return;
			}
			auto targetLocation = targetActor->GetPosition();


			auto distance = Vector3::Subtract(targetLocation, possessedPawn->GetPosition());
			float lengthSq = Vector3::LengthSquared(distance);
			float acceptableRadiusSq = AcceptableRadius * AcceptableRadius;


			if (lengthSq <= acceptableRadiusSq)
			{
				FinishExecute(true);
				return;
			}

			auto direction = Vector3::Normalize(distance);

			possessedPawn->AddForce(Vector3::Multiply(direction, MaxAcceleration));
		}

		STATIC_TYPE_IMPLE(BTTaskNodeMoveToActor)

	public:
		float AcceptableRadius = 0.0f;
		float MaxAcceleration = 0.0f;
		std::string TargetActorKey;
	};

	class BTTaskNodeMoveToLocation : public BTTaskNode
	{
	public:
		BTTaskNodeMoveToLocation() = default;
		~BTTaskNodeMoveToLocation() override = default;


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


			if (TargetLocationKey.empty())
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
			auto targetLocation = blackBoard->GetValueAsFloat3(TargetLocationKey);


			auto distance = Vector3::Subtract(targetLocation, possessedPawn->GetPosition());
			float lengthSq = Vector3::LengthSquared(distance);
			float acceptableRadiusSq = AcceptableRadius * AcceptableRadius;


			if (lengthSq <= acceptableRadiusSq)
			{
				FinishExecute(true);
				return;
			}

			auto direction = Vector3::Normalize(distance);

			possessedPawn->AddForce(Vector3::Multiply(direction, MaxAcceleration));
		}

		STATIC_TYPE_IMPLE(BTTaskNodeMoveToLocation)

	public:
		float AcceptableRadius = 0.0f;
		float MaxAcceleration = 0.0f;
		std::string TargetLocationKey;

	};


	class BTTaskNodeWait : public BTTaskNode
	{
	public:
		BTTaskNodeWait() = default;
		~BTTaskNodeWait() override = default;


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

			m_Timer.Tick();
			if (m_Timer.GetElapsedTime() >= WaitTime)
			{
				m_Timer.Reset();
				FinishExecute(true);
			}
		}

		STATIC_TYPE_IMPLE(BTTaskNodeWait)

	public:
		float WaitTime = 5.0f;

	private:

		Timer m_Timer = {};
	};


	class BTTaskNodeFaceLocation : public BTTaskNode
	{
	public:
		BTTaskNodeFaceLocation() = default;
		~BTTaskNodeFaceLocation() override = default;


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


			if (TargetLocationKey.empty())
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

			auto mainComponent = possessedPawn->GetMainComponent().lock();
			if (!mainComponent)
			{
				FinishExecute(false);
				return;
			}


			auto blackBoard = GetBlackboardComponent().lock();
			auto targetLocation = blackBoard->GetValueAsFloat3(TargetLocationKey);


			auto newFacing = Crystal::Vector3::Normalize(Crystal::Vector3::Subtract(targetLocation, mainComponent->GetWorldPosition()));

			auto normalizedForward = Crystal::Vector3::Normalize(mainComponent->GetWorldForwardVector());
			auto rotationAxis = Crystal::Vector3::Normalize(Crystal::Vector3::Cross(normalizedForward, newFacing));

			if (Vector3::IsZero(rotationAxis) || Vector3::Equal(normalizedForward, newFacing, 0.001f))
			{
				FinishExecute(true);
				return;
			}

			auto angle = Vector3::AngleBetweenNormals(normalizedForward, newFacing);
			if (fabs(angle) < DirectX::XMConvertToRadians(TargetAngleTolerance))
			{
				FinishExecute(true);
				return;
			}


			auto quat = Vector4::QuaternionRotationAxis(rotationAxis, angle);

			auto rotation = mainComponent->GetRotationQuat();
			auto newQuat = Vector4::QuaternionMultiply(rotation, quat);
			mainComponent->SetRotationQuat(newQuat);
		}

		STATIC_TYPE_IMPLE(BTTaskNodeFaceLocation)


	public:
		std::string TargetLocationKey;
		float TargetAngleTolerance = 0.0f;
	};



	class BTTaskNodeFaceActor : public BTTaskNode
	{
	public:
		BTTaskNodeFaceActor() = default;
		~BTTaskNodeFaceActor() override = default;


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


			if (TargetActorKey.empty())
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

			auto mainComponent = possessedPawn->GetMainComponent().lock();
			if (!mainComponent)
			{
				FinishExecute(false);
				return;
			}

			auto blackBoard = GetBlackboardComponent().lock();
			
			auto targetActor = Cast<Actor>(blackBoard->GetValueAsObject(TargetActorKey));
			if(!targetActor)
			{
				FinishExecute(false);
				return;
			}

			auto targetLocation = targetActor->GetPosition();

			auto newFacing = Crystal::Vector3::Normalize(Crystal::Vector3::Subtract(targetLocation, mainComponent->GetWorldPosition()));

			auto normalizedForward = Crystal::Vector3::Normalize(mainComponent->GetWorldForwardVector());
			auto rotationAxis = Crystal::Vector3::Normalize(Crystal::Vector3::Cross(normalizedForward, newFacing));

			if (Vector3::IsZero(rotationAxis) || Vector3::Equal(normalizedForward, newFacing, 0.001f))
			{
				FinishExecute(true);
				return;
			}

			auto angle = Vector3::AngleBetweenNormals(normalizedForward, newFacing);
			if (fabs(angle) < DirectX::XMConvertToRadians(TargetAngleTolerance))
			{
				FinishExecute(true);
				return;
			}


			auto quat = Vector4::QuaternionRotationAxis(rotationAxis, angle);

			auto rotation = mainComponent->GetRotationQuat();
			auto newQuat = Vector4::QuaternionMultiply(rotation, quat);
			mainComponent->SetRotationQuat(newQuat);


		}

		STATIC_TYPE_IMPLE(BTTaskNodeFaceActor)


	public:
		std::string TargetActorKey;
		float TargetAngleTolerance = 0.0f;
	};


	class BTTaskNodeClearBlackboardValue : public BTTaskNode
	{
	public:
		BTTaskNodeClearBlackboardValue() = default;
		~BTTaskNodeClearBlackboardValue() override = default;

		void Execute(float deltaTime) override
		{
			auto blackboard = GetBlackboardComponent().lock();
			blackboard->ClearValue(BlackboardKey);

			FinishExecute(true);
		}


		STATIC_TYPE_IMPLE(BTTaskNodeClearBlackboardValue)

	public:
		std::string BlackboardKey;
	};


	class BehaviorTree : public Object
	{
	public:
		BehaviorTree() = default;
		~BehaviorTree() override = default;

		void OnCreate() override
		{
			Object::OnCreate();

			m_RootNode = CreateObject<BTCompositeNode>("BTTreeRootNode", weak_from_this());
		}

		void Update(float deltaTime) override
		{
			Object::Update(deltaTime);
			m_RootNode->Execute(deltaTime);
		}

		Shared<BTCompositeNode> GetRootNode() const { return m_RootNode; }

		STATIC_TYPE_IMPLE(BehaviorTree)

	private:
		Shared<BTCompositeNode> m_RootNode = nullptr;
	};

	class BehaviorTreeComponent : public Component
	{
	public:
		BehaviorTreeComponent() = default;
		~BehaviorTreeComponent() override = default;

		void SetBehaviorTree(Shared<BehaviorTree> behaviorTree);

		void Update(float deltaTime) override
		{
			Component::Update(deltaTime);

			if (m_BehaviorTree)
			{
				m_BehaviorTree->Update(deltaTime);
			}
		}

		STATIC_TYPE_IMPLE(BehaviorTreeComponent)

	private:
		Shared<BehaviorTree> m_BehaviorTree = nullptr;
	};


}
