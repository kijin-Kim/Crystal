#pragma once
#include <vector>
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/InputComponent.h"

namespace Crystal {
	class Controller
	{
	public:
		Controller() = default;
		virtual ~Controller() = default;

		virtual void Possess(Pawn* pawn) {}

		virtual bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	};
}
