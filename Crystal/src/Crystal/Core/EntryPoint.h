#pragma once
#include <windows.h>
//#include <wrl/wrappers/corewrappers.h>

extern Crystal::Application* Crystal::CreateApplication(HINSTANCE hInstance);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

//#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
//	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
//	CS_ASSERT(SUCCEEDED(initialize), "Fail to Initialize COM");
//	// error
//#else
//	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
//	CS_ASSERT(SUCCEEDED(hr), "Fail to Initialize COM");
//	// error
//#endif
//		

	auto app = Crystal::CreateApplication(hInstance);
	app->Run();
	delete app;
}