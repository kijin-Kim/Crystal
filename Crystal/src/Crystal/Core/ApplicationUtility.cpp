#include "cspch.h"
#include "ApplicationUtility.h"
#include "Crystal/Gameplay/Controllers/PlayerController.h"

namespace Crystal {
	StateStack ApplicationUtility::s_StateStack;
	PlayerController* ApplicationUtility::s_PlayerController = nullptr;

	void ApplicationUtility::Init()
	{
		s_PlayerController = new PlayerController();
	}

	StateStack& ApplicationUtility::GetStateStack()
	{
		return s_StateStack;
	}

	void ApplicationUtility::PushState(State* State)
	{
		s_StateStack.PushState(State);
	}

	void ApplicationUtility::PopState(State* State)
	{
		s_StateStack.PopState(State);
	}

	PlayerController* ApplicationUtility::GetPlayerController()
	{
		return s_PlayerController;
	}
}