#include "cspch.h"
#include "Application.h"
#include "Crystal/Renderer/Renderer.h"
#include "DirectXTex/DirectXTex.h"


#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768


namespace Crystal {


	Application::Application(HINSTANCE hInstance, int width, int height) :
		m_Window(new WindowsWindow(hInstance, width, height))
	{
		/////// Manual Call ////
		auto& renderer = Renderer::Instance();
		renderer.Init(m_Window);
		m_Window->SetInputEventFunction(this, &Application::OnInputEvent);
	
	}

	void Application::Run()
	{
		while (m_bShouldRun)
		{
			MSG msg;
			while (PeekMessage(&msg, m_Window->GetWindowHandle(), 0, 0, PM_REMOVE))
			{
				if (!TranslateAcceleratorW(m_Window->GetWindowHandle(), nullptr, &msg))
				{
					if (msg.message == WM_QUIT)
						m_bShouldRun = false;
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			
			OnUpdate();
			Renderer::Instance().Render();
		}

	}

	void Application::OnUpdate()
	{
		m_MainTimer.Tick();
		for (State* state : m_StateStack)
			state->Update(m_MainTimer.DeltaTime());
	}

	void Application::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_MOUSEMOVE:
			break;
		case 'W':
			__debugbreak();
			break;
		case WM_KEYDOWN:
			break;
		case WM_KEYUP:
			break;
		default:
			break;
		}
	}

}

