#pragma once
#include "Controller.h"

namespace Crystal {

	class PlayerController : public Controller
	{
	public:
		PlayerController() = default;
		virtual ~PlayerController() = default;
		
		virtual void Possess(Pawn* pawn) override
		{
			Controller::Possess(pawn);
			m_InputComponents.push(new InputComponent());
			pawn->SetupInputComponent(m_InputComponents.top());
		}

	};
}
