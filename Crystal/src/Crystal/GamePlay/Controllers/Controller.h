#pragma once
#include <vector>
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/InputComponent.h"

namespace Crystal {
	class Controller : public Actor
	{
	public:
		Controller(Object* parent) : Actor(parent) { }
		virtual ~Controller() = default;

		virtual void Possess(Pawn* pawn) {}

		virtual bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	};
}
