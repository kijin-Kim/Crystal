#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <memory>

#include "WindowsWindow.h"
#include "Crystal/Renderer/Shader.h"
#include "Crystal/Renderer/Buffers.h"
#include "Crystal/Renderer/RenderTarget.h"
#include "Crystal/Renderer/DepthStencil.h"
#include "Crystal/Renderer/Pipeline.h"
#include "Crystal/Renderer/CommandList.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/Core/State.h"


//----
#include "Timer.h"
//----

namespace Crystal {

	class Application
	{
	public:
		Application(HINSTANCE hInstance, int width, int height);
		virtual ~Application() = default;

		void Run();

		virtual void OnUpdate();

		void OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


	private:
		bool m_bShouldRun = true;
		WindowsWindow* m_Window;
		Timer m_MainTimer;
	};

	Application* CreateApplication(HINSTANCE hInstance);
}


#define Register_Application(x, width, height) Crystal::Application* Crystal::CreateApplication(HINSTANCE hInstance)\
{\
	Crystal::ApplicationUtility::Init();\
	return new x(hInstance, width, height);\
}