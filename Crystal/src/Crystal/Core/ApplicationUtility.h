#pragma once
#include "Layer.h"

namespace Crystal {
	/*전역 유틸리티 제공 클래스*/

	class PlayerController;

	class ApplicationUtility
	{
	public:
		static PlayerController& GetPlayerController();

	private:
		ApplicationUtility() = delete;
		~ApplicationUtility() = delete;
		static PlayerController* s_PlayerController;
	};
}
