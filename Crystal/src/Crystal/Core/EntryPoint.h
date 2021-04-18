#pragma once
#include <windows.h>

extern Crystal::Application* Crystal::CreateApplication();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	auto app = Crystal::CreateApplication();
	app->Run();
	delete app;
}
