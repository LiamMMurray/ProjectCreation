#pragma once
#include <MemoryLeakDetection.h>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <DirectXMath.h>
#include <d3d11_1.h>
#pragma comment(lib, "d3d11.lib")

#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>
#include <algorithm>

#include <DirectXMacros.h>
#include <D3DNativeTypes.h>
#include <GlobalMacros.h>
#include <GEngine.h>
#include <ECS.h>
#include <MathLibrary.h>
#include <TransformComponent.h>
#include <XTime.h>
#include <SpookyHashV2.h>
#include <ResourceManager.h>