#pragma once

#include "DF_Settings.h"

//Common includes
#include <windows.h>
#include <wrl.h>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <exception>
#include <string>
#include <sstream>
#include <queue>
#include <bitset>
#include <optional>
#include <memory>
#include <unordered_map>
#include <math.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

//Direct3D
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <d3dcompiler.h>

//ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

//ASSIMP
#pragma comment (lib, "assimp-vc143-mt.lib")
#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

//D3D11 DDS Loader
#include "DFactory/Util/DDSTextureLoader11.h"

using namespace DirectX;

#ifdef _DEBUG
#pragma comment (lib, "dxguid.lib")
#include <dxgidebug.h>
#endif

#ifdef _CONSOLEAPP
#include <iostream>
#endif