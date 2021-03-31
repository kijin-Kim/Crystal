#include "cspch.h"
#include "ApplicationUtility.h"
#include "Crystal/Gameplay/Controllers/PlayerController.h"

namespace Crystal {
	LayerStack ApplicationUtility::s_LayerStack;
	PlayerController* ApplicationUtility::s_PlayerController = nullptr;

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
		if (s_PlayerController == nullptr)
			s_PlayerController = new PlayerController();
		return *s_PlayerController;
	}
}