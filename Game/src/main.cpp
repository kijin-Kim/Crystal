#include <Crystal.h>
#include <Crystal/Core/EntryPoint.h>
#include "MainLayer.h"

#define WINDOW_WIDTH 1366
#define WINDOW_HEIGHT 768


class GameApplication : public Crystal::Application
{
public:
	GameApplication(HINSTANCE hIsntance, int width, int height) : Crystal::Application(hIsntance, width, height)
	{
		Crystal::ApplicationUtility::PushLayer(new MainLayer());
	}

	/*virtual void OnUpdate() {}
	virtual void OnRender() {}
	virtual void OnEvent() {}*/

};

Register_Application(GameApplication, WINDOW_WIDTH, WINDOW_HEIGHT);