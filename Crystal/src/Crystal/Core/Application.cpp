#include "cspch.h"
#include "Application.h"
#include "Crystal/Renderer/Renderer.h"
#include "DirectXTex/DirectXTex.h"
#include "ApplicationUtility.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"


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
		for (State* state : ApplicationUtility::GetStateStack())
			state->Update(m_MainTimer.DeltaTime());
	}

	void Application::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		/*Process Global Window Events*/
		switch (uMsg)
		{
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

		ApplicationUtility::GetPlayerController()->OnInputEvent(hWnd, uMsg, wParam, lParam);
	}

}

