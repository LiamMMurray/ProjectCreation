#pragma once
#include "ForwardDeclarations/D3DNativeTypes.h"

namespace RenderUtility
{
        void UpdateConstantBuffer(ID3D11DeviceContext1* context, ID3D11Buffer* gpuBuffer, void* cpuBuffer, size_t size);
}