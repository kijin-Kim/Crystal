#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl/client.h>

#include "WindowsWindow.h"
#include "Crystal/Renderer/Shader.h"
#include "Crystal/Renderer/Buffers.h"
#include "Crystal/Renderer/RenderTarget.h"
#include "Crystal/Renderer/DepthStencil.h"
#include "Crystal/Renderer/Pipeline.h"
#include "Crystal/Renderer/Camera.h"
#include "Crystal/Renderer/CommandList.h"
#include "Crystal/Renderer/CommandQueue.h"

#include "Events.h"

//----
#include "Timer.h"
//----

namespace Crystal {

	class Application
	{
	public:
		Application(HINSTANCE hInstance, int width, int height);
		virtual ~Application() {}

		void Run();

		virtual void OnUpdate() {}
		virtual void OnEvent(Event& event);

	private:
		bool m_bShouldRun = true;
		std::shared_ptr<WindowsWindow> m_Window;
	};

	static Application* CreateApplication(HINSTANCE hInstance);
}


#define Register_Application(x, width, height) static Crystal::Application* Crystal::CreateApplication(HINSTANCE hInstance)\
{\
	return new x(hInstance, width, height);\
}