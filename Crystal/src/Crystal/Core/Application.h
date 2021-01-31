#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <memory>

#include "WindowsWindow.h"
#include "Crystal/Renderer/Shader.h"
#include "Crystal/Resources/Buffers.h"
#include "Crystal/Renderer/CommandList.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/Core/Layer.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/Core/Timer.h"

namespace Crystal {
	class Application
	{
	public:
		Application(HINSTANCE hInstance, int width, int height);
		virtual ~Application() = default;

		virtual void Start() {}

		void Run();

		virtual void OnUpdate() {}

		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		bool m_bShouldRun = true;
		WindowsWindow* m_Window;
		
	};

	Application* CreateApplication(HINSTANCE hInstance);
}

#define Register_Application(x, width, height) Crystal::Application* Crystal::CreateApplication(HINSTANCE hInstance)\
{\
	return new x(hInstance, width, height);\
}