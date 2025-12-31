#include "cspch.h"
#include "Decorator.h"
#include "Blackboard.h"
#include "BehaviorTree.h"

namespace Crystal {


	Weak<BlackboardComponent> Decorator::GetBlackboardComponent()
	{
		auto btNode = Cast<BTCompositeNode>(GetOuter());
		if(btNode)
		{
			return btNode->GetBlackboardComponent();
		}

		return {};
	}
}


