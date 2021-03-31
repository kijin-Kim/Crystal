#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgidebug.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>
#include <string>
#include <initializer_list>
#include <queue>
#include <vector>
#include <map>
#include <unordered_map>
#include <filesystem>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "Crystal/Core/Logger.h"
#include "DirectXTex/d3dx12.h"

#define SAFE_RELEASE(p) if(p) { p->Release(); p = nullptr;}