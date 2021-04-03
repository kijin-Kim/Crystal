#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <memory>

#include "WindowsWindow.h"
#include "Crystal/Resources/Shader.h"
#include "Crystal/Resources/Buffers.h"
#include "Crystal/Renderer/CommandList.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/Core/Layer.h"
#include "Crystal/GamePlay/World/World.h"
#include "Crystal/Core/Timer.h"
#include "Crystal/Resources/BasicShapeMeshes.h"

namespace Crystal {
	class Application
	{
	public:
		Application(HINSTANCE hInstance, int width, int height);
		virtual ~Application() = default;

		virtual void Start()
		{
			// Default Resources
			auto& resourceManager = Crystal::ResourceManager::Instance();
			resourceManager.CreateRenderable<Crystal::Line>("LineMesh");
			resourceManager.CreateRenderable<Crystal::LineBox>("LineBoxMesh");
			resourceManager.CreateRenderable<Crystal::LineSphere>("LineSphereMesh");
			resourceManager.CreateRenderable<Crystal::PlaneQuad>("PlaneQuadMesh");
		}

		void Run();

		virtual void Update() {}

		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	
	private:
		bool m_bShouldRun = true;
		WindowsWindow* m_Window;

	protected:
		std::shared_ptr<World> m_World;
	};

	Application* CreateApplication(HINSTANCE hInstance);
}

#define Register_Application(x, width, height) Crystal::Application* Crystal::CreateApplication(HINSTANCE hInstance)\
{\
	return new x(hInstance, width, height);\
}