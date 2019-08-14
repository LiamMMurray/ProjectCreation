#pragma once
#include <DirectXMath.h>

struct FInstanceData
{
        DirectX::XMMATRIX mtx;
        uint32_t          flags;
        float             lifeTime;
};
