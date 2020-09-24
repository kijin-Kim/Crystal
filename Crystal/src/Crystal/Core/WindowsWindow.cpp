#include "cspch.h"
#include "WindowsWindow.h"
#include "Application.h"

#include "examples/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Crystal {

	LRESULT CALLBACK WndProcProxy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static WindowsWindow* windowsWindow = nullptr;
		switch (uMsg)
		{
		case WM_NCCREATE:
		{
			const CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
			windowsWindow = reinterpret_cast<WindowsWindow*>(createStruct->lpCreateParams);
			break;
		}
		default:
			if (windowsWindow)
				return windowsWindow->WndProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	WindowsWindow::WindowsWindow(HINSTANCE hInstance, int width, int height) :
		m_Width(width), 
		m_Height(height)
	{
		WNDCLASS wndClass;

		wndClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
		wndClass.lpfnWndProc = WndProcProxy;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = NULL;
		wndClass.hCursor = NULL;
		wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = L"Hello Direct3d 12!";

		RegisterClass(&wndClass);

		m_Handle = CreateWindowEx(
			NULL,
			L"Hello Direct3d 12!",
			L"Hello Application!",
			WS_OVERLAPPEDWINDOW,
			0, 0, width, height,
			NULL, NULL, hInstance, this);

		CS_ASSERT(m_Handle, "윈도우를 생성하는데 실패하였습니다");

		ShowWindow(m_Handle, SW_SHOW);
		SetFocus(m_Handle);
	}

	WindowsWindow::~WindowsWindow()
	{
		DestroyWindow(m_Handle);
	}


	LRESULT WindowsWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			
			break;
		case WM_KEYDOWN:
			//m_EventCallbackFn(KeyEvent(wParam, KeyEvent::KeyState::Pressed));
			break;
		case WM_KEYUP:
			//m_EventCallbackFn(KeyEvent(wParam, KeyEvent::KeyState::Released));
			break;
		case WM_CLOSE:
			__debugbreak(); //Should Make Window Close Event to Destroy Main Game Loop
			break;
		case WM_QUIT:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void WindowsWindow::SetEventCallbackFn(Application* application, const std::function<void(Application*, Event&)>& function)
	{
		m_EventCallbackFn = std::bind(function, application, std::placeholders::_1);
	}

}


