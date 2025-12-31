#include "cspch.h"
#include "BehaviorTree.h"
#include "Crystal/GamePlay/Controllers/AIController.h"
#include "Decorator.h"
#include "Blackboard.h"

namespace Crystal {


	void BTCompositeNode::AddDecorator(const Shared<Decorator>& decorator)
	{
		decorator->SetOuter(weak_from_this());
		m_Decorators.push_back(decorator);
	}

	void BTCompositeNode::AddAbortDecorator(Weak<Decorator> decWeak)
	{
		m_AbortDecorators.push_back(decWeak);
		decWeak.lock()->SetOwnerNode(Cast<BTCompositeNode>(weak_from_this()));
	}

	void BTCompositeNode::AddAbortDecoratorToChilds(Weak<Decorator> decWeak)
	{
		for (auto& childNode : m_ChildNodes)
		{
			childNode->AddAbortDecorator(decWeak);
		}
	}

	void BTCompositeNode::PrepareAbortDecorators()
	{
		for (auto& child : m_ChildNodes)
		{
			child->PrepareAbortDecorators();
		}

		if (GetHasDecorators())
		{
			auto& decorators = GetDecorators();
			for (auto& decorator : decorators)
			{
				auto abortType = decorator->GetDecoratorAbortType();
				switch (abortType)
				{
				case EDecoratorAbortType::DAT_None: break;
				case EDecoratorAbortType::DAT_Self:
					{
						AddAbortDecorator(decorator);
					}
					break;
				case EDecoratorAbortType::DAT_LowerPriority:
					{
						auto parentNode = Cast<BTCompositeNode>(GetParentNode());

						auto& siblings = parentNode->GetChildNodes();
						auto it = std::find(siblings.begin(), siblings.end(), Cast<BTCompositeNode>(shared_from_this()));
						++it;
						for (it; it != siblings.end(); ++it)
						{
							(*it)->AddAbortDecorator(decorator);
							(*it)->AddAbortDecoratorToChilds(decorator);
						}
					}
					break;
				case EDecoratorAbortType::DAT_Both:
					{
						auto parentNode = Cast<BTCompositeNode>(GetParentNode());

						auto& siblings = parentNode->GetChildNodes();
						auto it = std::find(siblings.begin(), siblings.end(), Cast<BTCompositeNode>(shared_from_this()));
						for (it; it != siblings.end(); ++it)
						{
							(*it)->AddAbortDecorator(decorator);
							(*it)->AddAbortDecoratorToChilds(decorator);
						}
					}
					break;
				default:
					break;
					CS_FATAL(false, "");
				}
			}
		}
	}


	Weak<BTCompositeNode> BTCompositeNode::GetRootNode()
	{
		Shared<BTCompositeNode> parentNode = Cast<BTCompositeNode>(GetParentNode());
		Shared<BTCompositeNode> root = Cast<BTCompositeNode>(shared_from_this());
		while (parentNode)
		{
			root = parentNode;
			parentNode = Cast<BTCompositeNode>(parentNode->GetParentNode());
		}

		return root;
	}

	Weak<BehaviorTree> BTCompositeNode::GetBehaviorTree()
	{
		auto rootNode = GetRootNode().lock();
		if (rootNode)
		{
			return Cast<BehaviorTree>(rootNode->GetOuter());
		}

		return {};
	}

	Weak<BehaviorTreeComponent> BTCompositeNode::GetBehaviorTreeComponent()
	{
		auto behaviorTree = GetBehaviorTree().lock();
		if (behaviorTree)
		{
			return Cast<BehaviorTreeComponent>(behaviorTree->GetOuter());
		}

		return {};
	}

	Weak<AIController> BTCompositeNode::GetAIController()
	{
		auto behaviorTreeComponent = GetBehaviorTreeComponent().lock();
		if (behaviorTreeComponent)
		{
			return Cast<AIController>(behaviorTreeComponent->GetOuter());
		}

		return {};
	}

	Weak<Pawn> BTCompositeNode::GetPossessedPawn()
	{
		auto aiController = GetAIController().lock();
		if (aiController)
		{
			return aiController->GetPossessedPawn();
		}

		return {};
	}

	Weak<BlackboardComponent> BTCompositeNode::GetBlackboardComponent()
	{
		auto aiController = GetAIController().lock();
		if (aiController)
		{
			return aiController->GetBlackboardComponent();
		}

		return {};
	}

	bool BTCompositeNode::ExecuteDecorators() const
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

	bool BTCompositeNode::CheckAbortDecorators() const
	{
		if (m_AbortDecorators.empty())
		{
			return true;
		}

		for (auto& dec : m_AbortDecorators)
		{
			auto abortDec = dec.lock();
			if (!abortDec)
			{
				continue;
			}

			bool result = abortDec->Execute();
			switch (abortDec->GetAbortCondition())
			{
			case EAbortCondition::AC_OnResultChange:
				{
					bool bResultIsChanaged = result != abortDec->GetLastResult();
					if(bResultIsChanaged)
					{
						return false;
					}
				}
				break;
			case EAbortCondition::AC_OnValueChange:
				{
					bool bValueIsDifferent = abortDec->GetCurrentValueID() != abortDec->GetLastValueID();

					if(bValueIsDifferent)
					{
						abortDec->SetLastValueID(abortDec->GetCurrentValueID());
						return false;
					}
					else
					{
					}
				}
				break;
			}
		}

		return true;
	}


	void BehaviorTreeComponent::SetBehaviorTree(Shared<BehaviorTree> behaviorTree)
	{
		m_BehaviorTree = behaviorTree;
		m_BehaviorTree->SetOuter(weak_from_this());


		m_BehaviorTree->GetRootNode()->PrepareAbortDecorators();
	}
}


