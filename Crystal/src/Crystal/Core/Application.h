#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <memory>

#include "WindowsWindow.h"
#include "Crystal/Resources/Shader.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/Core/Timer.h"
#include "Crystal/Resources/BasicShapeMeshes.h"

namespace Crystal {
	class Application
	{
	public:
		Application(int width, int height);
		virtual ~Application() = default;

		virtual void Start()
		{
			// Default Resources
		}

		void Run();

		virtual void Update();

		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


		void ChangeResolution(int width, int height) const;
		void ChangeDisplayMode() const;
	
	protected:
		bool m_bShouldRun = true;
		std::unique_ptr<WindowsWindow> m_Window;
		std::shared_ptr<World> m_World;

		Shared<Timer> m_MainTimer = nullptr;
	};

	Application* CreateApplication();
}

#define Register_Application(x, width, height) Crystal::Application* Crystal::CreateApplication()\
{\
	return new x(width, height);\
}