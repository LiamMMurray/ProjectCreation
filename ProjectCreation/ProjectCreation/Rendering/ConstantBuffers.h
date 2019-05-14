#pragma once

#include <DirectXMath.h>

struct CTransformBuffer
{
        DirectX::XMMATRIX ViewProjection;
        DirectX::XMMATRIX World;
};