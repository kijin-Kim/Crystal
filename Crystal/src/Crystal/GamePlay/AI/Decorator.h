#pragma once
#include "Blackboard.h"
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {

	class BlackboardComponent;

	class Decorator : public Object
	{
	public:
		Decorator() = default;
		~Decorator() override = default;

		Weak<BlackboardComponent> GetBlackboardComponent();

		virtual bool Execute() { return true; }

		STATIC_TYPE_IMPLE(Decorator)
	};

	class BlackboardBasedDecorator : public Decorator
	{
	public:
		BlackboardBasedDecorator() = default;
		~BlackboardBasedDecorator() override = default;

		bool Execute() override
		{
			if(BlackboardKey.empty())
			{
				return false;
			}
			
			auto blackboard = GetBlackboardComponent().lock();
			if (!blackboard)
			{
				return false;
			}

			if(bIsSet)
			{
				return blackboard->HasValue(BlackboardKey);	
			}
			else
			{
				return !blackboard->HasValue(BlackboardKey);
			}
		}
	
	public:
		std::string BlackboardKey;
		bool bIsSet = true;
	};
}
