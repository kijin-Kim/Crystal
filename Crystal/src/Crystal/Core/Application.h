#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <memory>

#include "WindowsWindow.h"
#include "Crystal/Resources/Shader.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/Core/Layer.h"
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
			/*auto& resourceManager = ResourceManager::Instance();
			resourceManager.CreateRenderable<Line>("LineMesh");
			resourceManager.CreateRenderable<LineBox>("LineBoxMesh");
			resourceManager.CreateRenderable<LineSphere>("LineSphereMesh");
			resourceManager.CreateRenderable<PlaneQuad2D>("2DPlaneQuadMesh");
			resourceManager.CreateRenderableFromFile<StaticMesh>("assets/models/Sphere.fbx", "Sphere");*/
		}

		void Run();

		virtual void Update() {}

		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


		void ChangeResolution(int width, int height) const;
		void ChangeDisplayMode() const;
	
	protected:
		bool m_bShouldRun = true;
		std::unique_ptr<WindowsWindow> m_Window;
		std::shared_ptr<World> m_World;
	};

	Application* CreateApplication();
}

#define Register_Application(x, width, height) Crystal::Application* Crystal::CreateApplication()\
{\
	return new x(width, height);\
}