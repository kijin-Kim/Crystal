#pragma once
#include "Blackboard.h"
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {
	class BTCompositeNode;

	class BlackboardComponent;

	enum class EDecoratorAbortType
	{
		DAT_None,
		DAT_Self,
		DAT_LowerPriority,
		DAT_Both
	};

	enum class EAbortCondition
	{
		AC_OnResultChange,
		AC_OnValueChange,
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


		void SetLastValueID(uint64_t bID) { m_bLastValueID = bID; }
		uint64_t GetLastValueID() const { return m_bLastValueID; }
		uint64_t GetCurrentValueID() const { return m_bCurrentValueID; }
		

		EDecoratorAbortType GetDecoratorAbortType() const { return AbortType; }
		EAbortCondition GetAbortCondition() const { return AbortCondition; }

		void SetOwnerNode(Weak<BTCompositeNode> ownerNode) { m_OwnerNode = ownerNode; }
		Weak<BTCompositeNode> GetOwnerNode() const { return m_OwnerNode; }

		STATIC_TYPE_IMPLE(Decorator)

	protected:
		uint64_t m_bLastValueID = 0;
		uint64_t m_bCurrentValueID = 0;
		bool m_bLastResult = false;
		bool m_bCurrentResult = false;
		Weak<BTCompositeNode> m_OwnerNode;


	public:
		EDecoratorAbortType AbortType = EDecoratorAbortType::DAT_None;
		EAbortCondition AbortCondition = EAbortCondition::AC_OnResultChange;
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
				bool hasValue = blackboard->HasValue(BlackboardKey);
				if(hasValue)
				{
					m_bCurrentValueID = blackboard->GetValueID(BlackboardKey);
				}
				else
				{
					m_bCurrentValueID = 0;
				}

				m_bCurrentResult = hasValue;
				return m_bCurrentResult;
			}
			else
			{
				bool hasValue = blackboard->HasValue(BlackboardKey);
				if(hasValue)
				{
					m_bCurrentValueID = blackboard->GetValueID(BlackboardKey);
				}
				else
				{
					m_bCurrentValueID = 0;
				}

				m_bCurrentResult = !hasValue;
				return m_bCurrentResult;
			}
		}

	public:
		std::string BlackboardKey;
		bool bIsSet = true;
	};
}


