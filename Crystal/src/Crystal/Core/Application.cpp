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
		
		m_Window->SetEventCallbackFn(this, &Application::OnEvent);
	}

	void Application::Run()
	{
		while (m_bShouldRun)
		{
			MSG msg;
			while (PeekMessage(&msg, m_Window->GetWindowHandle(), 0, 0, PM_REMOVE))
			{
				if (!TranslateAccelerator(m_Window->GetWindowHandle(), m_Window->GetAccelTable(), &msg))
				{
					if (msg.message == WM_QUIT)
						m_bShouldRun = false;
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			OnUpdate();
			Renderer::Get().Render();
		}

	}

	void Application::OnEvent(Event& event)
	{
		//Handle Application Level Event
		//World
		

		//Handle Mode Level Event
		// for (auto world : m_Worlds)
		//		bool isHandled = world->OnEvent();
		//		if(isHandled)
		//			break;



		/*SetActionInput("MoveForward", VK_RETURN);
		ActionMap[VK_RETURN] = "MoveForward"
		ActionFunctionMap["MoveForward"] = std::bind(function, this, std::placeholders::_1);



		PlayerInputComponent->BindAxis("MoveForward", this, &Application::MoveForward);
		

		"MoveForward"
*/
	}

}

