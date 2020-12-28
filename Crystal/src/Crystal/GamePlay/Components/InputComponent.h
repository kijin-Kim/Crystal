#pragma once
#include <functional>
#include "Component.h"

namespace Crystal {

#define CS_AXIS_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define CS_ACTION_FN(fn) std::bind(&fn, this)

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

		void BindAxis(const std::string& axisName, const std::function<void(float value)>& function);
		void BindAction(const std::string& actionName, EKeyStatus keyEventType, const std::function<void(void)>& function);

		bool ProcessInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		/* ActionName, Function */
		std::map<std::pair<std::string, EKeyStatus>, std::function<void(void)>> m_ActionFunctionMap;
		std::map<std::string, std::function<void(float)>> m_AxisFunctionmap;
	};

}