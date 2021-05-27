#include "cspch.h"
#include "Application.h"
#include "DirectXTex/DirectXTex.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "imgui.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"


#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768

namespace Crystal {
	Application::Application(int width, int height)
	{
#ifndef CS_NM_DEDICATED
		m_Window = std::make_unique<WindowsWindow>(width, height);
		m_Window->SetInputEventFunction(this, &Application::OnInputEvent);
#endif


		m_World = std::make_shared<World>();
		m_World->OnCreate();
		m_World->SetObjectName("DefaultWorld");
		m_World->CreateNewLevel("DefaultLevel");
		m_World->SetCurrentLevelByName("DefaultLevel");

		
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		Start();
		while (m_bShouldRun)
		{
			int msgCount = 0;
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
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
			Update();
		}
	}

	void Application::Update()
	{
		m_Timer.Tick();
		m_World->Update(m_Timer.GetDeltaSeconds());
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
				//Renderer::Instance().ActiveFullScreenMode(!Renderer::Instance().GetIsFullScreenMode());
			}
			return true;
		}

		/*IMGUI가 처리하지 않는 일부 데이터만 Application이 처리합니다.*/
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureKeyboard && !io.WantCaptureMouse)
		{
			auto currentLevel = m_World->GetCurrentLevel();
			if (!currentLevel)
			{
				return false;
			}

			if (m_World)
			{
				m_World->OnInputEvent(hWnd, uMsg, wParam, lParam);
			}
		}
		return false;
	}

	void Application::ChangeResolution(int width, int height) const
	{
		if (auto currentLevel = m_World->GetCurrentLevel())
		{
			auto renderSystem = Cast<RenderSystem>(currentLevel->GetActorByClass("RenderSystem"));
			renderSystem->ChangeResolution(width, height);
		}

		m_Window->SetWidth(width);
		m_Window->SetHeight(height);
	}

	void Application::ChangeDisplayMode() const
	{
		if (auto currentLevel = m_World->GetCurrentLevel())
		{
			auto renderSystem = Cast<RenderSystem>(currentLevel->GetActorByClass("RenderSystem"));
			renderSystem->ChangeDisplayMode();
		}
	}
}
