#pragma once
#include <functional>
#include <windows.h>

namespace Crystal {
	class Application;

	class WindowsWindow final
	{
	public:

		WindowsWindow(HINSTANCE hInstance, int width, int height);
		~WindowsWindow();

		int GetDisplayResolutionWidth() const { return GetSystemMetrics(SM_CXSCREEN); }
		int GetDisplayResolutionHeight() const { return GetSystemMetrics(SM_CYSCREEN); }

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		void SetWidth(int width) { m_Width = width; }
		void SetHeight(int height) { m_Height = height; }

		const HWND GetWindowHandle() const { return m_Handle; }

		LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void SetInputEventFunction(Application* app, const std::function<bool(Application*, HWND, UINT, WPARAM, LPARAM)>& function);

	private:
		HWND m_Handle = nullptr;
		int m_Width = 0;
		int m_Height = 0;

		std::function<bool(HWND, UINT, WPARAM, LPARAM)> m_InputEventFunction = nullptr;
	};
};
