#pragma once
#include "State.h"

namespace Crystal {

	/*전역 유틸리티 제공 클래스*/
	
	class PlayerController;

	class ApplicationUtility
	{
	public:
		static void Init();
		static StateStack& GetStateStack();
		static void PushState(State* State);
		static void PopState(State* State);

		static PlayerController* GetPlayerController();
		
	private:
		ApplicationUtility() = delete;
		~ApplicationUtility() = delete;

	private:
		static StateStack s_StateStack;
		static PlayerController* s_PlayerController;
	};

}
