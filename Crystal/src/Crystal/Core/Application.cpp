#include "cspch.h"
#include "Application.h"
#include "Crystal/Renderer/Renderer.h"
#include "DirectXTex/DirectXTex.h"


#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768


namespace Crystal {

	Application::Application(HINSTANCE hInstance, int width, int height) :
		m_Window(std::make_shared<WindowsWindow>(hInstance, width, height))
	{
		/////// Manual Call ////
		auto& renderer = Renderer::Get();
		renderer.Init(m_Window);
	}

	void Application::Run()
	{
		while (m_bShouldRun)
		{
			MSG msg;
			while (PeekMessage(&msg, m_Window->GetWindowHandle(), 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					m_bShouldRun = false;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			OnUpdate();
			Renderer::Get().Render();
		}

	}

}

