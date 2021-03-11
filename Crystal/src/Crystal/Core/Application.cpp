#include "cspch.h"
#include "Application.h"
#include "Crystal/Renderer/Renderer.h"
#include "DirectXTex/DirectXTex.h"
#include "ApplicationUtility.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "imgui.h"

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
		Start();
		while (m_bShouldRun)
		{
			int msgCount = 0;
			MSG msg;
			while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					m_bShouldRun = false;
				TranslateMessage(&msg);
				DispatchMessage(&msg);

				const int maxMessageCount = 5;
				++msgCount;
				if (msgCount >= maxMessageCount)
					break;
			}
			OnUpdate();
			Renderer::Instance().Render();
			
		}
	}

	bool Application::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		/*Process Global Window Events*/

		switch (uMsg)
		{
		case WM_CLOSE:
			m_bShouldRun = false;
			return true;
		case WM_QUIT:
			DestroyWindow(hWnd);
			return true;
		case WM_DESTROY:
			PostQuitMessage(0);
			return true;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				m_bShouldRun = false;
				DestroyWindow(hWnd);
			}
			if (wParam == VK_F11)
			{
				Renderer::Instance().ActiveFullScreenMode(!Renderer::Instance().GetIsFullScreenMode());
			}
			return true;
		}

		/*IMGUI가 처리하지 않는 일부 데이터만 Application이 처리합니다.*/
		ImGuiIO& io = ImGui::GetIO();
		if(!io.WantCaptureKeyboard && !io.WantCaptureMouse)
			return ApplicationUtility::GetPlayerController().OnInputEvent(hWnd, uMsg, wParam, lParam);
		return false;
	}
}