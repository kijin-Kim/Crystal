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
			pawn->SetupInputComponent(m_InputComponents.top());
		}

	};
}
