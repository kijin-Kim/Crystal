#include "cspch.h"
#include "ApplicationUtility.h"
#include "Crystal/Gameplay/Controllers/PlayerController.h"

namespace Crystal {
	LayerStack ApplicationUtility::s_LayerStack;
	PlayerController ApplicationUtility::s_PlayerController;


	LayerStack& ApplicationUtility::GetLayerStack()
	{
		return s_LayerStack;
	}

	void ApplicationUtility::PushLayer(Layer* layer)
	{
		s_LayerStack.PushLayer(layer);
	}

	void ApplicationUtility::PopLayer(Layer* layer)
	{
		s_LayerStack.PopLayer(layer);
	}

	PlayerController& ApplicationUtility::GetPlayerController()
	{
		return s_PlayerController;
	}
}