#pragma once
#include <vector>
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/InputComponent.h"

namespace Crystal {
	class Controller : public Actor
	{
	public:
		Controller() = default;
		virtual ~Controller() = default;

		virtual void Possess(std::shared_ptr<Pawn> pawn) {}

		virtual bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }

		STATIC_TYPE_IMPLE(Controller)
	};
}
