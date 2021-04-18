#pragma once
#include "Layer.h"

namespace Crystal {
	/*���� ��ƿ��Ƽ ���� Ŭ����*/

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
