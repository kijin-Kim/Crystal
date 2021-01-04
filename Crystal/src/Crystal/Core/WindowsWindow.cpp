#include "cspch.h"
#include "WindowsWindow.h"
#include "Application.h"


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
		ShowCursor(false);
	}

	WindowsWindow::~WindowsWindow()
	{
		DestroyWindow(m_Handle);
	}

	LRESULT WindowsWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (m_InputEventFunction && !m_InputEventFunction(hWnd, uMsg, wParam, lParam))
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		else
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void WindowsWindow::SetInputEventFunction(Application* app, const std::function<bool(Application*, HWND, UINT, WPARAM, LPARAM)>& function)
	{
		m_InputEventFunction = std::bind(function, app, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}

}