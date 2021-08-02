#include "cspch.h"
#include "BehaviorTree.h"
#include "Crystal/GamePlay/Controllers/AIController.h"
#include "Decorator.h"
#include "Blackboard.h"

namespace Crystal {


	void BTNode::AddDecorator(const Shared<Decorator>& decorator)
	{
		decorator->SetOuter(weak_from_this());
		m_Decorators.push_back(decorator);
	}

	Weak<BTCompositeNode> BTNode::GetRootNode()
	{
		Shared<BTCompositeNode> parentNode = Cast<BTCompositeNode>(GetParentNode());
		Shared<BTCompositeNode> root = Cast<BTCompositeNode>(shared_from_this());
		while(parentNode)
		{
			root = parentNode;
			parentNode = Cast<BTCompositeNode>(parentNode->GetParentNode());
		}
		
		return root;
	}

	Weak<BehaviorTree> BTNode::GetBehaviorTree()
	{
		auto rootNode = GetRootNode().lock();
		if (rootNode)
		{
			return Cast<BehaviorTree>(rootNode->GetOuter());
		}

		return {};
	}

	Weak<BehaviorTreeComponent> BTNode::GetBehaviorTreeComponent()
	{
		auto behaviorTree = GetBehaviorTree().lock();
		if (behaviorTree)
		{
			return Cast<BehaviorTreeComponent>(behaviorTree->GetOuter());
		}

		return {};
	}

	Weak<AIController> BTNode::GetAIController()
	{
		auto behaviorTreeComponent = GetBehaviorTreeComponent().lock();
		if (behaviorTreeComponent)
		{
			return Cast<AIController>(behaviorTreeComponent->GetOuter());
		}

		return {};
	}

	Weak<Pawn> BTNode::GetPossessedPawn()
	{
		auto aiController = GetAIController().lock();
		if (aiController)
		{
			return aiController->GetPossessedPawn();
		}

		return {};
	}

	Weak<BlackboardComponent> BTNode::GetBlackboardComponent()
	{
		auto aiController = GetAIController().lock();
		if (aiController)
		{
			return aiController->GetBlackboardComponent();
		}

		return {};
	}

	bool BTNode::ExecuteDecorators() const
	{
		if (m_Decorators.empty())
		{
			return true;
		}

		for (auto& dec : m_Decorators)
		{
			bool result = dec->Execute();
			if (!result)
			{
				return false;
			}
				
		}

		return true;
	}


	void BehaviorTreeComponent::SetBehaviorTree(Shared<BehaviorTree> behaviorTree)
	{
		m_BehaviorTree = behaviorTree;
		m_BehaviorTree->SetOuter(weak_from_this());
	}
}
