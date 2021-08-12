#pragma once
#include "Blackboard.h"
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {

	class BlackboardComponent;

	enum class EDecoratorAbortType
	{
		DAT_None,
		DAT_Self,
		DAT_LowerPriority,
		DAT_Both
	};

	class Decorator : public Object
	{
	public:
		Decorator() = default;
		~Decorator() override = default;

		Weak<BlackboardComponent> GetBlackboardComponent();

		virtual bool Execute()
		{
			m_bLastResult = true;
			return true;
		}

		bool GetLastResult() const { return m_bLastResult; }

		EDecoratorAbortType GetDecoratorAbortType() const { return AbortType; }

		STATIC_TYPE_IMPLE(Decorator)

	protected:
		bool m_bLastResult = false;
		bool m_bCurrentResult = false;

	public:
		EDecoratorAbortType AbortType = EDecoratorAbortType::DAT_None;
	};

	class BlackboardBasedDecorator : public Decorator
	{
	public:
		BlackboardBasedDecorator() = default;
		~BlackboardBasedDecorator() override = default;

		bool Execute() override
		{
			m_bLastResult = m_bCurrentResult;

			if (BlackboardKey.empty())
			{
				m_bCurrentResult = false;
				return false;
			}

			auto blackboard = GetBlackboardComponent().lock();
			if (!blackboard)
			{
				m_bCurrentResult = false;
				return false;
			}

			if (bIsSet)
			{
				m_bCurrentResult = blackboard->HasValue(BlackboardKey);
				return m_bCurrentResult;
			}
			else
			{
				m_bCurrentResult = !blackboard->HasValue(BlackboardKey);
				return m_bCurrentResult;
			}
		}

	public:
		std::string BlackboardKey;
		bool bIsSet = true;
	};
}
