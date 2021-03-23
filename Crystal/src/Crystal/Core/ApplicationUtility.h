#pragma once
#include "Layer.h"

namespace Crystal {
	/*전역 유틸리티 제공 클래스*/

	class PlayerController;

	class ApplicationUtility
	{
	public:
		static LayerStack& GetLayerStack();
		static void PushLayer(Layer* layer);
		static void PopLayer(Layer* layer);

		static PlayerController& GetPlayerController();

	private:
		ApplicationUtility() = delete;
		~ApplicationUtility() = delete;

	private:
		static LayerStack s_LayerStack;
		static PlayerController s_PlayerController;
	};
}
