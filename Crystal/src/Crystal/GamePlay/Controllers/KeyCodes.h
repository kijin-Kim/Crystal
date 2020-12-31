#pragma once
#include <windowsx.h>
///// This KeyCodes and Mapping functions are from SFML https://github.com/SFML/SFML/blob/master/include/SFML/Window/Keyboard.hpp#L144
///// https://github.com/SFML/SFML/blob/master/include/SFML/Window/Mouse.hpp

namespace Crystal {

	struct MouseCodeWithDelta
	{
		std::pair<int64_t, float> MouseX;
		std::pair<int64_t, float> MouseY;
		std::pair<int64_t, float> VWheel;
		std::pair<int64_t, float> HWheel;
	};

	enum class EKeyEvent
	{
		/*Keyboard Event*/
		KE_None,
		KE_Pressed,
		KE_Released,
		KE_Repeat
	};

	class Keyboard
	{
	public:
		enum Key
		{
			Unknown = -1, //!< Unhandled key
			A = 0,        //!< The A key
			B,            //!< The B key
			C,            //!< The C key
			D,            //!< The D key
			E,            //!< The E key
			F,            //!< The F key
			G,            //!< The G key
			H,            //!< The H key
			I,            //!< The I key
			J,            //!< The J key
			K,            //!< The K key
			L,            //!< The L key
			M,            //!< The M key
			N,            //!< The N key
			O,            //!< The O key
			P,            //!< The P key
			Q,            //!< The Q key
			R,            //!< The R key
			S,            //!< The S key
			T,            //!< The T key
			U,            //!< The U key
			V,            //!< The V key
			W,            //!< The W key
			X,            //!< The X key
			Y,            //!< The Y key
			Z,            //!< The Z key
			Num0,         //!< The 0 key
			Num1,         //!< The 1 key
			Num2,         //!< The 2 key
			Num3,         //!< The 3 key
			Num4,         //!< The 4 key
			Num5,         //!< The 5 key
			Num6,         //!< The 6 key
			Num7,         //!< The 7 key
			Num8,         //!< The 8 key
			Num9,         //!< The 9 key
			Escape,       //!< The Escape key
			LControl,     //!< The left Control key
			LShift,       //!< The left Shift key
			LAlt,         //!< The left Alt key
			LSystem,      //!< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
			RControl,     //!< The right Control key
			RShift,       //!< The right Shift key
			RAlt,         //!< The right Alt key
			RSystem,      //!< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
			Menu,         //!< The Menu key
			LBracket,     //!< The [ key
			RBracket,     //!< The ] key
			Semicolon,    //!< The ; key
			Comma,        //!< The , key
			Period,       //!< The . key
			Quote,        //!< The ' key
			Slash,        //!< The / key
			Backslash,    //!< The \ key
			Tilde,        //!< The ~ key
			Equal,        //!< The = key
			Hyphen,       //!< The - key (hyphen)
			Space,        //!< The Space key
			Enter,        //!< The Enter/Return keys
			Backspace,    //!< The Backspace key
			Tab,          //!< The Tabulation key
			PageUp,       //!< The Page up key
			PageDown,     //!< The Page down key
			End,          //!< The End key
			Home,         //!< The Home key
			Insert,       //!< The Insert key
			Delete,       //!< The Delete key
			Add,          //!< The + key
			Subtract,     //!< The - key (minus, usually from numpad)
			Multiply,     //!< The * key
			Divide,       //!< The / key
			Left,         //!< Left arrow
			Right,        //!< Right arrow
			Up,           //!< Up arrow
			Down,         //!< Down arrow
			Numpad0,      //!< The numpad 0 key
			Numpad1,      //!< The numpad 1 key
			Numpad2,      //!< The numpad 2 key
			Numpad3,      //!< The numpad 3 key
			Numpad4,      //!< The numpad 4 key
			Numpad5,      //!< The numpad 5 key
			Numpad6,      //!< The numpad 6 key
			Numpad7,      //!< The numpad 7 key
			Numpad8,      //!< The numpad 8 key
			Numpad9,      //!< The numpad 9 key
			F1,           //!< The F1 key
			F2,           //!< The F2 key
			F3,           //!< The F3 key
			F4,           //!< The F4 key
			F5,           //!< The F5 key
			F6,           //!< The F6 key
			F7,           //!< The F7 key
			F8,           //!< The F8 key
			F9,           //!< The F9 key
			F10,          //!< The F10 key
			F11,          //!< The F11 key
			F12,          //!< The F12 key
			F13,          //!< The F13 key
			F14,          //!< The F14 key
			F15,          //!< The F15 key
			Pause,        //!< The Pause key

			KeyCount,     //!< Keep last -- the total number of keyboard keys

			// Deprecated values:

			Dash = Hyphen,       //!< \deprecated Use Hyphen instead
			BackSpace = Backspace,    //!< \deprecated Use Backspace instead
			BackSlash = Backslash,    //!< \deprecated Use Backslash instead
			SemiColon = Semicolon,    //!< \deprecated Use Semicolon instead
			Return = Enter         //!< \deprecated Use Enter instead
		};
	};

	class Mouse
	{
	public:
		enum Button
		{
			UnKnown = 102,
			Left,       //!< The left mouse button
			Right,      //!< The right mouse button
			Middle,     //!< The middle (wheel) mouse button
			XButton1,   //!< The first extra mouse button
			XButton2,   //!< The second extra mouse button

			ButtonCount,//!< Keep last -- the total number of mouse buttons

			////////////////////////////////////////////////////////////
			/// \brief Mouse wheels
			///
			////////////////////////////////////////////////////////////
		};

		enum Wheel
		{
			VerticalWheel = 109,  //!< The vertical mouse wheel
			HorizontalWheel //!< The horizontal mouse wheel

			////////////////////////////////////////////////////////////
			/// \brief Check if a mouse button is pressed
			///
			/// \param button Button to check
			///
			/// \return True if the button is pressed, false otherwise
			///
		};

		enum Move
		{
			X = 111,
			Y
		};
	};
	

	static std::pair<int64_t, EKeyEvent> GetCrystalKeyCode(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		case VK_RWIN:       crystalKey.first =Keyboard::RSystem; break;
		case VK_APPS:       crystalKey.first =Keyboard::Menu; break;
		case VK_OEM_1:      crystalKey.first =Keyboard::Semicolon; break;
		case VK_OEM_2:      crystalKey.first =Keyboard::Slash; break;
		case VK_OEM_PLUS:   crystalKey.first =Keyboard::Equal; break;
		case VK_OEM_MINUS:  crystalKey.first =Keyboard::Hyphen; break;
		case VK_OEM_4:      crystalKey.first =Keyboard::LBracket; break;
		case VK_OEM_6:      crystalKey.first =Keyboard::RBracket; break;
		case VK_OEM_COMMA:  crystalKey.first =Keyboard::Comma; break;
		case VK_OEM_PERIOD: crystalKey.first =Keyboard::Period; break;
		case VK_OEM_7:      crystalKey.first =Keyboard::Quote; break;
		case VK_OEM_5:      crystalKey.first =Keyboard::Backslash; break;
		case VK_OEM_3:      crystalKey.first =Keyboard::Tilde; break;
		case VK_ESCAPE:     crystalKey.first =Keyboard::Escape; break;
		case VK_SPACE:      crystalKey.first =Keyboard::Space; break;
		case VK_RETURN:     crystalKey.first =Keyboard::Enter; break;
		case VK_BACK:       crystalKey.first =Keyboard::Backspace; break;
		case VK_TAB:        crystalKey.first =Keyboard::Tab; break;
		case VK_PRIOR:      crystalKey.first =Keyboard::PageUp; break;
		case VK_NEXT:       crystalKey.first =Keyboard::PageDown; break;
		case VK_END:        crystalKey.first =Keyboard::End; break;
		case VK_HOME:       crystalKey.first =Keyboard::Home; break;
		case VK_INSERT:     crystalKey.first =Keyboard::Insert; break;
		case VK_DELETE:     crystalKey.first =Keyboard::Delete; break;
		case VK_ADD:        crystalKey.first =Keyboard::Add; break;
		case VK_SUBTRACT:   crystalKey.first =Keyboard::Subtract; break;
		case VK_MULTIPLY:   crystalKey.first =Keyboard::Multiply; break;
		case VK_DIVIDE:     crystalKey.first =Keyboard::Divide; break;
		case VK_PAUSE:      crystalKey.first =Keyboard::Pause; break;
		case VK_F1:         crystalKey.first =Keyboard::F1; break;
		case VK_F2:         crystalKey.first =Keyboard::F2; break;
		case VK_F3:         crystalKey.first =Keyboard::F3; break;
		case VK_F4:         crystalKey.first =Keyboard::F4; break;
		case VK_F5:         crystalKey.first =Keyboard::F5; break;
		case VK_F6:         crystalKey.first =Keyboard::F6; break;
		case VK_F7:         crystalKey.first =Keyboard::F7; break;
		case VK_F8:         crystalKey.first =Keyboard::F8; break;
		case VK_F9:         crystalKey.first =Keyboard::F9; break;
		case VK_F10:        crystalKey.first =Keyboard::F10; break;
		case VK_F11:        crystalKey.first =Keyboard::F11; break;
		case VK_F12:        crystalKey.first =Keyboard::F12; break;
		case VK_F13:        crystalKey.first =Keyboard::F13; break;
		case VK_F14:        crystalKey.first =Keyboard::F14; break;
		case VK_F15:        crystalKey.first =Keyboard::F15; break;
		case VK_LEFT:       crystalKey.first =Keyboard::Left; break;
		case VK_RIGHT:      crystalKey.first =Keyboard::Right; break;
		case VK_UP:         crystalKey.first =Keyboard::Up; break;
		case VK_DOWN:       crystalKey.first =Keyboard::Down; break;
		case VK_NUMPAD0:    crystalKey.first =Keyboard::Numpad0; break;
		case VK_NUMPAD1:    crystalKey.first =Keyboard::Numpad1; break;
		case VK_NUMPAD2:    crystalKey.first =Keyboard::Numpad2; break;
		case VK_NUMPAD3:    crystalKey.first =Keyboard::Numpad3; break;
		case VK_NUMPAD4:    crystalKey.first =Keyboard::Numpad4; break;
		case VK_NUMPAD5:    crystalKey.first =Keyboard::Numpad5; break;
		case VK_NUMPAD6:    crystalKey.first =Keyboard::Numpad6; break;
		case VK_NUMPAD7:    crystalKey.first =Keyboard::Numpad7; break;
		case VK_NUMPAD8:    crystalKey.first =Keyboard::Numpad8; break;
		case VK_NUMPAD9:    crystalKey.first =Keyboard::Numpad9; break;
		case 'A':           crystalKey.first =Keyboard::A; break;
		case 'Z':           crystalKey.first =Keyboard::Z; break;
		case 'E':           crystalKey.first =Keyboard::E; break;
		case 'R':           crystalKey.first =Keyboard::R; break;
		case 'T':           crystalKey.first =Keyboard::T; break;
		case 'Y':           crystalKey.first =Keyboard::Y; break;
		case 'U':           crystalKey.first =Keyboard::U; break;
		case 'I':           crystalKey.first =Keyboard::I; break;
		case 'O':           crystalKey.first =Keyboard::O; break;
		case 'P':           crystalKey.first =Keyboard::P; break;
		case 'Q':           crystalKey.first =Keyboard::Q; break;
		case 'S':           crystalKey.first =Keyboard::S; break;
		case 'D':           crystalKey.first =Keyboard::D; break;
		case 'F':           crystalKey.first =Keyboard::F; break;
		case 'G':           crystalKey.first =Keyboard::G; break;
		case 'H':           crystalKey.first =Keyboard::H; break;
		case 'J':           crystalKey.first =Keyboard::J; break;
		case 'K':           crystalKey.first =Keyboard::K; break;
		case 'L':           crystalKey.first =Keyboard::L; break;
		case 'M':           crystalKey.first =Keyboard::M; break;
		case 'W':           crystalKey.first =Keyboard::W; break;
		case 'X':           crystalKey.first =Keyboard::X; break;
		case 'C':           crystalKey.first =Keyboard::C; break;
		case 'V':           crystalKey.first =Keyboard::V; break;
		case 'B':           crystalKey.first =Keyboard::B; break;
		case 'N':           crystalKey.first =Keyboard::N; break;
		case '0':           crystalKey.first =Keyboard::Num0; break;
		case '1':           crystalKey.first =Keyboard::Num1; break;
		case '2':           crystalKey.first =Keyboard::Num2; break;
		case '3':           crystalKey.first =Keyboard::Num3; break;
		case '4':           crystalKey.first =Keyboard::Num4; break;
		case '5':           crystalKey.first =Keyboard::Num5; break;
		case '6':           crystalKey.first =Keyboard::Num6; break;
		case '7':           crystalKey.first =Keyboard::Num7; break;
		case '8':           crystalKey.first =Keyboard::Num8; break;
		case '9':           crystalKey.first =Keyboard::Num9; break;
		}

		static bool bLeftButtonDown = false;
		static bool bRightButtonDown = false;
		static bool bMiddleButtonDown = false;
		static bool bXButton1Down = false;
		static bool bXButton2Down = false;

		switch (uMsg)
		{
		case WM_LBUTTONDOWN :
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

	static MouseCodeWithDelta GetCrystalMouseCodeWithDelta(UINT uMsg, WPARAM wParam)
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
			RECT rc = {};
			GetClientRect(GetActiveWindow(), &rc);
			static int prevX = (rc.right - rc.left) / 2.0f;
			static int prevY = (rc.bottom - rc.top) / 2.0f;
			int diffFromLastX = cursorPos.x - prevX;
			int diffFromLastY = cursorPos.y - prevY;
			SetCursorPos(prevX, prevY);

			codes.MouseX = { Mouse::X, diffFromLastX };
			codes.MouseY = { Mouse::Y, -diffFromLastY };
		}
		
		switch (uMsg)
		{
		case WM_MOUSEHWHEEL:
			codes.HWheel = { Mouse::HorizontalWheel, GET_WHEEL_DELTA_WPARAM(wParam) };
			break;
		case WM_MOUSEWHEEL:
			codes.VWheel = { Mouse::VerticalWheel, GET_WHEEL_DELTA_WPARAM(wParam) };
			break;
		}

		return codes;
		
	}



}
