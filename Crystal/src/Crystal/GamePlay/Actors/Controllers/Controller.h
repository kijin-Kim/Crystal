#pragma once
#include <stack>
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/InputComponent.h"

namespace Crystal {

	class Controller : public Actor
	{
	public:
		Controller() = default;
		virtual ~Controller()
		{
			while (!m_InputComponents.empty())
			{
				delete m_InputComponents.top();
				m_InputComponents.pop();
			}
		}

		virtual void Possess(Pawn* pawn) {}

		virtual void Update(float deltaTime) override
		{
			m_InputComponents.top()->Update(deltaTime);
		}

	protected:
		std::stack<InputComponent*> m_InputComponents;
	};


}
