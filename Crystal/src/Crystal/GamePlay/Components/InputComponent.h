#pragma once
#include <functional>

#include "Component.h"
#include "Crystal/GamePlay/Controllers/KeyCodes.h"

namespace Crystal {
#define CS_AXIS_FN(fn) std::bind(&fn, this, std::placeholders::_1)
#define CS_ACTION_FN(fn) std::bind(&fn, this)

	class Pawn;

	class InputComponent : public Component
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Component>(*this);
			ar & m_bShouldBindCursor;
			ar & m_bIsCursorVisible;

			ar & m_LastPosition;
			ar & m_BindPosition;
		}

	public:
		InputComponent() = default;
		virtual ~InputComponent() = default;

		void BindAxis(const std::string& axisName, const std::function<void(float value)>& function);
		void BindAction(const std::string& actionName, EKeyEvent keyEventType,
		                const std::function<void(void)>& function);
		void UnBindAction(const std::string& actionName, EKeyEvent keyEventType);

		bool ProcessInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


		void BindCursor(bool bShouldBindCursor) { m_bShouldBindCursor = bShouldBindCursor; }
		bool IsCursorBind() const { return m_bShouldBindCursor; }
		/*커서를 바인딩 하기전 바인딩할 위치를 지정합니다.*/
		void ReadyCursorBinding()
		{
			GetCursorPos(&m_BindPosition);
		}

		void ShowCursor(bool bShowCursor) { m_bIsCursorVisible = bShowCursor; }

		STATIC_TYPE_IMPLE(InputComponent)

	private:
		bool ProcessAxisMappedInput(int64_t keyCode, float axisValue);
		bool ProcessActionMappedInput(UINT uMsg, int64_t keyCode, LPARAM lParam, EKeyEvent keyStatus);
		std::pair<int64_t, EKeyEvent> GetCrystalKeyCode(UINT uMsg, WPARAM wParam, LPARAM lParam);
		MouseCodeWithDelta GetCrystalMouseCodeWithDelta(UINT uMsg, LPARAM lParam);
	private:
		/* ActionName, Function */
		std::map<std::pair<std::string, EKeyEvent>, std::function<void(void)>> m_ActionFunctionMap;
		std::map<std::string, std::function<void(float)>> m_AxisFunctionmap;

		bool m_bShouldBindCursor = false;
		bool m_bIsCursorVisible = true;

		POINT m_LastPosition = {};
		POINT m_BindPosition = {};
	};
}
