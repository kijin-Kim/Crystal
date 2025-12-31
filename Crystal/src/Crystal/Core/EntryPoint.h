#pragma once

extern Crystal::Application* Crystal::CreateApplication();


#ifdef CS_NM_DEDICATED
#pragma comment(linker, "/subsystem:console")
int main()
{
	auto app = Crystal::CreateApplication();
	app->Run();
	delete app;
}
#else
#include <windows.h>
#pragma comment(linker, "/subsystem:windows")
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	auto app = Crystal::CreateApplication();
	app->Run();
	delete app;
}
#endif




