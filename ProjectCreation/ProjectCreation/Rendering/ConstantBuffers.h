#pragma once

#include <DirectXMath.h>

struct CTransformBuffer
{
        DirectX::XMMATRIX ViewProjection;
        DirectX::XMMATRIX World;
};

struct CSceneInfoBuffer
{
        DirectX::XMFLOAT3 eyePosition;
        float             time;
        DirectX::XMFLOAT3 directionalLightDirection;
        float             pad;
        DirectX::XMFLOAT4 directioanlLightColor;
};

struct CAnimationBuffer
{
        DirectX::XMMATRIX jointTransforms[64];
};