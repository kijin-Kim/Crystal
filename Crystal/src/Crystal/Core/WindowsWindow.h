#pragma once
#include <functional>
#include <windows.h>
#include "Events.h"

namespace Crystal {

	class WindowsWindow final
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		WindowsWindow(HINSTANCE hInstance, int width, int height);
		~WindowsWindow();

		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }

		const HWND GetWindowHandle() const { return m_Handle; }

		LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		void SetEventCallbackFn(class Application* application, const std::function<void(class Application*, Event&)>& function);

		//Imple Future
		const HACCEL& GetAccelTable() const { return m_AccelTable; }

	private:
		HWND m_Handle = nullptr;
		int m_Width = 0;
		int m_Height = 0;

		HACCEL m_AccelTable = nullptr;
		EventCallbackFn m_EventCallbackFn = nullptr;
	};

};
