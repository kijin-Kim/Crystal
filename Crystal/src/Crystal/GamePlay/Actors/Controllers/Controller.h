#pragma once
#include <stack>
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/InputComponent.h"

namespace Crystal {

	class Controller
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

		virtual void Possess(Pawn* pawn) 
		{
			m_InputComponents.push(new InputComponent());
		}

		void OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{

		}


	protected:
		std::stack<InputComponent*> m_InputComponents;
	};


}
