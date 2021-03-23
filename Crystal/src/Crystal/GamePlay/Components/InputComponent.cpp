#include "cspch.h"
#include "InputComponent.h"
#include "Crystal/Core/ApplicationUtility.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

namespace Crystal {
	void InputComponent::BindAxis(const std::string& axisName, const std::function<void(float value)>& function)
	{
		m_AxisFunctionmap.insert(std::make_pair(axisName, function));
	}

	void InputComponent::BindAction(const std::string& actionName, EKeyEvent keyEventType, const std::function<void(void)>& function)
	{
		m_ActionFunctionMap.insert(std::make_pair(std::make_pair(actionName, keyEventType), function));
	}

	void InputComponent::UnBindAction(const std::string& actionName, EKeyEvent keyEventType)
	{
		auto key = std::make_pair(actionName, keyEventType);
		if (m_ActionFunctionMap.count(key))
		{
			m_ActionFunctionMap.erase(key);
		}
	}

	bool InputComponent::ProcessInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		/*윈도우 커서 카운팅 시스템에 따라 처리*/
	
		CURSORINFO cursorInfo = {};
		cursorInfo.cbSize = sizeof(CURSORINFO);
		GetCursorInfo(&cursorInfo);

		if (m_bIsCursorVisible && cursorInfo.flags != CURSOR_SHOWING)
		{
			while (::ShowCursor(true) < 0);
		}
		else if (!m_bIsCursorVisible && cursorInfo.flags == CURSOR_SHOWING)
		{
			while (::ShowCursor(false) >= 0);
		}
		

		/*액션 매핑을 처리합니다.*/
		auto [keyCode, keyStatus] = getCrystalKeyCode(uMsg, wParam, lParam);
		processActionMappedInput(uMsg, keyCode, lParam, keyStatus);

		MouseCodeWithDelta mouseCodes = getCrystalMouseCodeWithDelta(uMsg, lParam);
		processActionMappedInput(uMsg, mouseCodes.MouseX.first, lParam, EKeyEvent::KE_Pressed);
		processActionMappedInput(uMsg, mouseCodes.MouseY.first, lParam, EKeyEvent::KE_Pressed);
		processActionMappedInput(uMsg, mouseCodes.VWheel.first, lParam, EKeyEvent::KE_Pressed);
		processActionMappedInput(uMsg, mouseCodes.HWheel.first, lParam, EKeyEvent::KE_Pressed);



		/*액시스 매핑을 처리합니다.*/
		static UCHAR pKeysBuffer[256];
		for (int i = 0; i < 256; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				auto [keyCode, keyStatus] = getCrystalKeyCode(uMsg, i, lParam);
				processAxisMappedInput(keyCode, 1.0f);
			}
		}
		processAxisMappedInput(mouseCodes.MouseX.first, mouseCodes.MouseX.second);
		processAxisMappedInput(mouseCodes.MouseY.first, mouseCodes.MouseY.second);
		processAxisMappedInput(mouseCodes.VWheel.first, mouseCodes.VWheel.second);
		processAxisMappedInput(mouseCodes.HWheel.first, mouseCodes.HWheel.second);


		return false;
	}

	bool InputComponent::processAxisMappedInput(int64_t keyCode, float axisValue)
	{
		auto axisMap = ApplicationUtility::GetPlayerController().GetAxisMap();
		bool bHandledOnAxis = false;

		/*Process Axis*/
		auto axisIt = axisMap.find(keyCode);
		if (axisIt != axisMap.end())
		{
			std::string axisName = (*axisIt).second.first;
			float axisScale = (*axisIt).second.second;
			auto axisFnIt = m_AxisFunctionmap.find(axisName);
			if (axisFnIt != m_AxisFunctionmap.end())
			{
				bHandledOnAxis = true;
				auto axisFn = (*axisFnIt).second;
				/*Call Axis Function*/
				axisFn(axisValue * axisScale);
			}
		}
		return bHandledOnAxis;

	}

	bool InputComponent::processActionMappedInput(UINT uMsg, int64_t keyCode, LPARAM lParam, EKeyEvent keyStatus)
	{
		auto actionMap = ApplicationUtility::GetPlayerController().GetActionMap();

		bool bHandledOnAction = false;

		ActionMapping actionKey = {};
		actionKey.CrystalCode = keyCode;
		actionKey.bAltDown = GetKeyState(VK_MENU) & 0x8000;
		actionKey.bCtrlDown = GetKeyState(VK_CONTROL) & 0x8000;
		actionKey.bShiftDown = GetKeyState(VK_SHIFT) & 0x8000;


		/*Process Action*/
		auto actionIt = actionMap.find(actionKey);
		if (actionIt != actionMap.end())
		{
			std::string actionName = (*actionIt).second;
			auto actionFnIt = m_ActionFunctionMap.find(std::make_pair(actionName, keyStatus));
			if (actionFnIt != m_ActionFunctionMap.end())
			{
				bHandledOnAction = true;
				auto actionFn = (*actionFnIt).second;
				/*Call Action Function*/
				actionFn();
			}
		}

		return bHandledOnAction;
	}



	std::pair<int64_t, EKeyEvent> InputComponent::getCrystalKeyCode(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		std::pair<int64_t, EKeyEvent> crystalKey = std::make_pair(Keyboard::Unknown, EKeyEvent::KE_Pressed);
		crystalKey.second = (HIWORD(lParam) & KF_REPEAT) ? EKeyEvent::KE_Repeat : EKeyEvent::KE_Pressed;
		crystalKey.second = uMsg == WM_KEYUP ? EKeyEvent::KE_Released : crystalKey.second;

		switch (wParam)
		{
			// Check the scancode to distinguish between left and right shift
		case VK_SHIFT:
		{
			static UINT lShift = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
			UINT scancode = static_cast<UINT>((lParam & (0xFF << 16)) >> 16);
			if (scancode == lShift)
			{
				crystalKey.first = Keyboard::LShift;
			}
			else
			{
				crystalKey.first = Keyboard::RShift;
			}
			break;
		}

		// Check the "extended" flag to distinguish between left and right alt
		case VK_MENU:
			if ((HIWORD(lParam) & KF_EXTENDED))
				crystalKey.first = Keyboard::RAlt;
			else
				crystalKey.first = Keyboard::LAlt;
			break;
			// Check the "extended" flag to distinguish between left and right control
		case VK_CONTROL:
			if ((HIWORD(lParam) & KF_EXTENDED))
				crystalKey.first = Keyboard::RControl;
			else
				crystalKey.first = Keyboard::LControl;
			break;
			// Other keys are reported properly
		case VK_LWIN:       crystalKey.first = Keyboard::LSystem; break;
		case VK_RWIN:       crystalKey.first = Keyboard::RSystem; break;
		case VK_APPS:       crystalKey.first = Keyboard::Menu; break;
		case VK_OEM_1:      crystalKey.first = Keyboard::Semicolon; break;
		case VK_OEM_2:      crystalKey.first = Keyboard::Slash; break;
		case VK_OEM_PLUS:   crystalKey.first = Keyboard::Equal; break;
		case VK_OEM_MINUS:  crystalKey.first = Keyboard::Hyphen; break;
		case VK_OEM_4:      crystalKey.first = Keyboard::LBracket; break;
		case VK_OEM_6:      crystalKey.first = Keyboard::RBracket; break;
		case VK_OEM_COMMA:  crystalKey.first = Keyboard::Comma; break;
		case VK_OEM_PERIOD: crystalKey.first = Keyboard::Period; break;
		case VK_OEM_7:      crystalKey.first = Keyboard::Quote; break;
		case VK_OEM_5:      crystalKey.first = Keyboard::Backslash; break;
		case VK_OEM_3:      crystalKey.first = Keyboard::Tilde; break;
		case VK_ESCAPE:     crystalKey.first = Keyboard::Escape; break;
		case VK_SPACE:      crystalKey.first = Keyboard::Space; break;
		case VK_RETURN:     crystalKey.first = Keyboard::Enter; break;
		case VK_BACK:       crystalKey.first = Keyboard::Backspace; break;
		case VK_TAB:        crystalKey.first = Keyboard::Tab; break;
		case VK_PRIOR:      crystalKey.first = Keyboard::PageUp; break;
		case VK_NEXT:       crystalKey.first = Keyboard::PageDown; break;
		case VK_END:        crystalKey.first = Keyboard::End; break;
		case VK_HOME:       crystalKey.first = Keyboard::Home; break;
		case VK_INSERT:     crystalKey.first = Keyboard::Insert; break;
		case VK_DELETE:     crystalKey.first = Keyboard::Delete; break;
		case VK_ADD:        crystalKey.first = Keyboard::Add; break;
		case VK_SUBTRACT:   crystalKey.first = Keyboard::Subtract; break;
		case VK_MULTIPLY:   crystalKey.first = Keyboard::Multiply; break;
		case VK_DIVIDE:     crystalKey.first = Keyboard::Divide; break;
		case VK_PAUSE:      crystalKey.first = Keyboard::Pause; break;
		case VK_F1:         crystalKey.first = Keyboard::F1; break;
		case VK_F2:         crystalKey.first = Keyboard::F2; break;
		case VK_F3:         crystalKey.first = Keyboard::F3; break;
		case VK_F4:         crystalKey.first = Keyboard::F4; break;
		case VK_F5:         crystalKey.first = Keyboard::F5; break;
		case VK_F6:         crystalKey.first = Keyboard::F6; break;
		case VK_F7:         crystalKey.first = Keyboard::F7; break;
		case VK_F8:         crystalKey.first = Keyboard::F8; break;
		case VK_F9:         crystalKey.first = Keyboard::F9; break;
		case VK_F10:        crystalKey.first = Keyboard::F10; break;
		case VK_F11:        crystalKey.first = Keyboard::F11; break;
		case VK_F12:        crystalKey.first = Keyboard::F12; break;
		case VK_F13:        crystalKey.first = Keyboard::F13; break;
		case VK_F14:        crystalKey.first = Keyboard::F14; break;
		case VK_F15:        crystalKey.first = Keyboard::F15; break;
		case VK_LEFT:       crystalKey.first = Keyboard::Left; break;
		case VK_RIGHT:      crystalKey.first = Keyboard::Right; break;
		case VK_UP:         crystalKey.first = Keyboard::Up; break;
		case VK_DOWN:       crystalKey.first = Keyboard::Down; break;
		case VK_NUMPAD0:    crystalKey.first = Keyboard::Numpad0; break;
		case VK_NUMPAD1:    crystalKey.first = Keyboard::Numpad1; break;
		case VK_NUMPAD2:    crystalKey.first = Keyboard::Numpad2; break;
		case VK_NUMPAD3:    crystalKey.first = Keyboard::Numpad3; break;
		case VK_NUMPAD4:    crystalKey.first = Keyboard::Numpad4; break;
		case VK_NUMPAD5:    crystalKey.first = Keyboard::Numpad5; break;
		case VK_NUMPAD6:    crystalKey.first = Keyboard::Numpad6; break;
		case VK_NUMPAD7:    crystalKey.first = Keyboard::Numpad7; break;
		case VK_NUMPAD8:    crystalKey.first = Keyboard::Numpad8; break;
		case VK_NUMPAD9:    crystalKey.first = Keyboard::Numpad9; break;
		case 'A':           crystalKey.first = Keyboard::A; break;
		case 'Z':           crystalKey.first = Keyboard::Z; break;
		case 'E':           crystalKey.first = Keyboard::E; break;
		case 'R':           crystalKey.first = Keyboard::R; break;
		case 'T':           crystalKey.first = Keyboard::T; break;
		case 'Y':           crystalKey.first = Keyboard::Y; break;
		case 'U':           crystalKey.first = Keyboard::U; break;
		case 'I':           crystalKey.first = Keyboard::I; break;
		case 'O':           crystalKey.first = Keyboard::O; break;
		case 'P':           crystalKey.first = Keyboard::P; break;
		case 'Q':           crystalKey.first = Keyboard::Q; break;
		case 'S':           crystalKey.first = Keyboard::S; break;
		case 'D':           crystalKey.first = Keyboard::D; break;
		case 'F':           crystalKey.first = Keyboard::F; break;
		case 'G':           crystalKey.first = Keyboard::G; break;
		case 'H':           crystalKey.first = Keyboard::H; break;
		case 'J':           crystalKey.first = Keyboard::J; break;
		case 'K':           crystalKey.first = Keyboard::K; break;
		case 'L':           crystalKey.first = Keyboard::L; break;
		case 'M':           crystalKey.first = Keyboard::M; break;
		case 'W':           crystalKey.first = Keyboard::W; break;
		case 'X':           crystalKey.first = Keyboard::X; break;
		case 'C':           crystalKey.first = Keyboard::C; break;
		case 'V':           crystalKey.first = Keyboard::V; break;
		case 'B':           crystalKey.first = Keyboard::B; break;
		case 'N':           crystalKey.first = Keyboard::N; break;
		case '0':           crystalKey.first = Keyboard::Num0; break;
		case '1':           crystalKey.first = Keyboard::Num1; break;
		case '2':           crystalKey.first = Keyboard::Num2; break;
		case '3':           crystalKey.first = Keyboard::Num3; break;
		case '4':           crystalKey.first = Keyboard::Num4; break;
		case '5':           crystalKey.first = Keyboard::Num5; break;
		case '6':           crystalKey.first = Keyboard::Num6; break;
		case '7':           crystalKey.first = Keyboard::Num7; break;
		case '8':           crystalKey.first = Keyboard::Num8; break;
		case '9':           crystalKey.first = Keyboard::Num9; break;
		}

		static bool bLeftButtonDown = false;
		static bool bRightButtonDown = false;
		static bool bMiddleButtonDown = false;
		static bool bXButton1Down = false;
		static bool bXButton2Down = false;

		switch (uMsg)
		{
		case WM_LBUTTONDOWN:
		{
			crystalKey.first = Mouse::Left;
			if (bLeftButtonDown)
			{
				crystalKey.second = EKeyEvent::KE_Repeat;
			}
			else
			{
				bLeftButtonDown = true;
				crystalKey.second = EKeyEvent::KE_Pressed;
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			bLeftButtonDown = false;
			crystalKey.first = Mouse::Left;
			crystalKey.second = EKeyEvent::KE_Released;
			break;
		}
		case WM_RBUTTONDOWN:
		{
			crystalKey.first = Mouse::Right;
			if (bRightButtonDown)
			{
				crystalKey.second = EKeyEvent::KE_Repeat;
			}
			else
			{
				bRightButtonDown = true;
				crystalKey.second = EKeyEvent::KE_Pressed;
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			bRightButtonDown = false;
			crystalKey.first = Mouse::Right;
			crystalKey.second = EKeyEvent::KE_Released;
			break;
		}
		case WM_MBUTTONDOWN:
		{
			crystalKey.first = Mouse::Middle;
			if (bMiddleButtonDown)
			{
				crystalKey.second = EKeyEvent::KE_Repeat;
			}
			else
			{
				bMiddleButtonDown = true;
				crystalKey.second = EKeyEvent::KE_Pressed;
			}
			break;
		}
		case WM_MBUTTONUP:
		{
			bMiddleButtonDown = false;
			crystalKey.first = Mouse::Middle;
			crystalKey.second = EKeyEvent::KE_Released;
			break;
		}
		case WM_XBUTTONDOWN:
		{
			switch (wParam)
			{
			case VK_XBUTTON1:
				crystalKey.first = Mouse::XButton1;
				if (bXButton1Down)
				{
					crystalKey.second = EKeyEvent::KE_Repeat;
				}
				else
				{
					bXButton1Down = true;
					crystalKey.second = EKeyEvent::KE_Pressed;
				}
				break;
			case VK_XBUTTON2:
				crystalKey.first = Mouse::XButton2;
				if (bXButton2Down)
				{
					crystalKey.second = EKeyEvent::KE_Repeat;
				}
				else
				{
					bXButton2Down = true;
					crystalKey.second = EKeyEvent::KE_Pressed;
				}
				break;
			}
			break;
		}
		case WM_XBUTTONUP:
			switch (wParam)
			{
			case VK_XBUTTON1:
				crystalKey.first = Mouse::XButton1;
				bXButton1Down = false;
				crystalKey.second = EKeyEvent::KE_Released;
				break;
			case VK_XBUTTON2:
				crystalKey.first = Mouse::XButton2;
				bXButton2Down = false;
				crystalKey.second = EKeyEvent::KE_Released;
				break;
			}
			break;
		default:
			break;
		}

		return  crystalKey;
	}

	MouseCodeWithDelta InputComponent::getCrystalMouseCodeWithDelta(UINT uMsg, LPARAM lParam)
	{
		MouseCodeWithDelta codes;
		codes.MouseX = { Mouse::UnKnown, 0.0f };
		codes.MouseY = { Mouse::UnKnown, 0.0f };
		codes.HWheel = { Mouse::UnKnown, 0.0f };
		codes.VWheel = { Mouse::UnKnown, 0.0f };

		if (uMsg == WM_MOUSEMOVE)
		{
			POINT cursorPos = {};
			GetCursorPos(&cursorPos);
			
			if (m_bShouldBindCursor)
			{
				m_LastPosition.x = m_BindPosition.x;
				m_LastPosition.y = m_BindPosition.y;
				SetCursorPos(m_LastPosition.x, m_LastPosition.y);
			}
			
			POINT diffFromLast = {};
			diffFromLast.x = cursorPos.x - m_LastPosition.x;
			diffFromLast.y = cursorPos.y - m_LastPosition.y;
				
			m_LastPosition = cursorPos;

			codes.MouseX = { Mouse::X, (float)diffFromLast.x };
			codes.MouseY = { Mouse::Y, -(float)diffFromLast.y};
		}

		switch (uMsg)
		{
		case WM_MOUSEHWHEEL:
			codes.HWheel = { Mouse::HorizontalWheel, GET_WHEEL_DELTA_WPARAM(lParam) };
			break;
		case WM_MOUSEWHEEL:
			codes.VWheel = { Mouse::VerticalWheel, GET_WHEEL_DELTA_WPARAM(lParam) };
			break;
		}

		return codes;

	}
}