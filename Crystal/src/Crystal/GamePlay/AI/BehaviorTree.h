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

	
	class BTNode : public Object
	{
	public:
		BTNode() = default;
		~BTNode() override = default;

		void SetParentNode(Weak<BTNode> parentNode) { m_ParentNode = parentNode; }
		void AddDecorator(const Shared<Decorator>& decorator);
		
		Weak<BTNode> GetParentNode() const { return m_ParentNode; }
		Weak<BTCompositeNode> GetRootNode();
		Weak<BehaviorTree> GetBehaviorTree();
		Weak<BehaviorTreeComponent> GetBehaviorTreeComponent();
		Weak<AIController> GetAIController();
		Weak<Pawn> GetPossessedPawn();
		Weak<BlackboardComponent> GetBlackboardComponent();
		

		virtual bool Execute(float deltaTime)
		{
			return false;
		}

		bool ExecuteDecorators() const;


		STATIC_TYPE_IMPLE(BTNode)

	protected:
		Weak<BTNode> m_ParentNode;
		std::vector<Shared<Decorator>> m_Decorators;
	};
	
	class BTCompositeNode : public BTNode
	{
	public:
		BTCompositeNode() = default;
		~BTCompositeNode() override = default;

		void AddChildNode(Shared<BTNode> childNode)
		{
			m_ChildNodes.push_back(childNode);
			childNode->SetParentNode(Cast<BTNode>(weak_from_this()));
		}

		virtual bool Execute(float deltaTime)
		{
			

			bool result = ExecuteDecorators();
			if(!result)
			{
				return false;
			}

			for (auto& childNode : m_ChildNodes)
			{
				childNode->Execute(deltaTime);
			}

			
			return true;
		}
		
	
		STATIC_TYPE_IMPLE(BTCompositeNode)

	protected:
		std::vector<Shared<BTNode>> m_ChildNodes;
	};

	class BTSelectorNode : public BTCompositeNode
	{
	public:
		BTSelectorNode() = default;
		~BTSelectorNode() override = default;

		bool Execute(float deltaTime) override
		{
			

			bool result = ExecuteDecorators();
			if (!result)
			{
				return false;
			}

			for (auto& childNode : m_ChildNodes)
			{
				bool result = childNode->Execute(deltaTime);
				if(result)
				{
					return true;
				}
			}
			// if all node failed
			return false;
		}

		STATIC_TYPE_IMPLE(BTSelectorNode)
	};

	class BTSequenceNode : public BTCompositeNode
	{
	public:
		BTSequenceNode() = default;
		~BTSequenceNode() override = default;

		bool Execute(float deltaTime) override
		{
			

			bool result = ExecuteDecorators();
			if (!result)
			{
				return false;
			}

			for (auto& childNode : m_ChildNodes)
			{
				bool result = childNode->Execute(deltaTime);
				if (!result)
				{
					return false;
				}
			}
			// if all node succeed
			return true;
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

	class BTTaskNodeMoveToLocation : public BTTaskNode
	{
	public:
		BTTaskNodeMoveToLocation() = default;
		~BTTaskNodeMoveToLocation() override = default;

		
		bool Execute(float deltaTime) override
		{
			

			bool result = ExecuteDecorators();
			if (!result)
			{
				return false;
			}
			

			if (TargetLocationKey.empty())
			{
				return false;
			}

			auto possessedPawn = GetPossessedPawn().lock();
			if (!possessedPawn)
			{
				return false;
			}


			if(bArrivedAtTargetLocation)
			{
				return true;
			}
				
			auto blackBoard = GetBlackboardComponent().lock();
			auto targetLocation = blackBoard->GetValueAsFloat3(TargetLocationKey);


			auto distance = Vector3::Subtract(targetLocation, possessedPawn->GetPosition());
			float lengthSq = Vector3::LengthSquared(distance);
			float acceptableRadiusSq = AcceptableRadius * AcceptableRadius;

			
			if (lengthSq <= acceptableRadiusSq)
			{
				bArrivedAtTargetLocation = true;
				return true;
			}

			auto direction = Vector3::Normalize(distance);

			possessedPawn->AddForce(Vector3::Multiply(direction, MaxAcceleration));

			return false;
		}
		
		STATIC_TYPE_IMPLE(BTTaskNodeMoveToLocation)

	public:
		float AcceptableRadius = 0.0f;
		float MaxAcceleration = 0.0f;
		std::string TargetLocationKey;
		
		bool bArrivedAtTargetLocation = false;
	};


	class BTTaskNodeFaceLocation : public BTTaskNode
	{
	public:
		BTTaskNodeFaceLocation() = default;
		~BTTaskNodeFaceLocation() override = default;


		bool Execute(float deltaTime) override
		{		
			

			bool result = ExecuteDecorators();
			if (!result)
			{
				return false;
			}

			
			if (TargetLocationKey.empty())
			{
				return false;
			}

			auto possessedPawn = GetPossessedPawn().lock();
			if (!possessedPawn)
			{
				return false;
			}

			auto mainComponent = possessedPawn->GetMainComponent().lock();
			if (!mainComponent)
			{
				return false;
			}
			

			auto blackBoard = GetBlackboardComponent().lock();
			auto targetLocation = blackBoard->GetValueAsFloat3(TargetLocationKey);

			auto newFacing = Crystal::Vector3::Normalize(Crystal::Vector3::Subtract(targetLocation, mainComponent->GetWorldPosition()));

			auto normalizedForward = Crystal::Vector3::Normalize(mainComponent->GetWorldForwardVector());
			auto rotationAxis = Crystal::Vector3::Normalize(Crystal::Vector3::Cross(normalizedForward, newFacing));

			if (Vector3::IsZero(rotationAxis) || Vector3::Equal(normalizedForward, newFacing, 0.001f))
			{
				return true;
			}

			auto angle = Vector3::AngleBetweenNormals(normalizedForward, newFacing);
			auto quat = Vector4::QuaternionRotationAxis(rotationAxis, angle);

			auto rotation = mainComponent->GetRotationQuat();
			auto newQuat = Vector4::QuaternionMultiply(rotation, quat);
			mainComponent->SetRotationQuat(newQuat);

			return false;
		}

		STATIC_TYPE_IMPLE(BTTaskNodeFaceLocation)

	
	public:
		std::string TargetLocationKey;
	};

	class BTTaskNodeClearBlackboardValue : public BTTaskNode
	{
	public:
		BTTaskNodeClearBlackboardValue() = default;
		~BTTaskNodeClearBlackboardValue() override = default;

		bool Execute(float deltaTime) override
		{
			auto blackboard = GetBlackboardComponent().lock();
			blackboard->ClearValue(BlackboardKey);

			return true;
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

			if(m_BehaviorTree)
			{
				m_BehaviorTree->Update(deltaTime);
			}
			
		}

		STATIC_TYPE_IMPLE(BehaviorTreeComponent)

	private:
		Shared<BehaviorTree> m_BehaviorTree = nullptr;
	};


}
