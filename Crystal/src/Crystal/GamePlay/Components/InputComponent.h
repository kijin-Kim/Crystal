#pragma once
#include <functional>
#include "Component.h"

namespace Crystal {

	class Pawn;

	enum class EKeyStatus
	{
		KS_None,
		KS_Pressed,
		KS_Released,
		KS_Repeat
	};
	

	class InputComponent : public Component
	{
	public:
		InputComponent() = default;
		virtual ~InputComponent() = default;

		void BindAxis(const std::string& axisName, Pawn* owner, const std::function<void(Pawn*, float value)>& function);
		void BindAction(const std::string& actionName, EKeyStatus keyEventType, Pawn* owner, const std::function<void(Pawn*)>& function);

		bool ProcessInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		/* ActionName, Function */
		std::map<std::pair<std::string, EKeyStatus>, std::function<void(void)>> m_ActionFunctionMap;
		std::map<std::string, std::function<void(float)>> m_AxisFunctionmap;
	};

}