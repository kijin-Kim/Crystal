#include "cspch.h"
#include "ApplicationUtility.h"
#include "Crystal/Gameplay/Controllers/PlayerController.h"

namespace Crystal {

	PlayerController* ApplicationUtility::s_PlayerController = nullptr;

	PlayerController& ApplicationUtility::GetPlayerController()
	{
		if (s_PlayerController == nullptr)
			s_PlayerController = new PlayerController();
		return *s_PlayerController;
	}
}