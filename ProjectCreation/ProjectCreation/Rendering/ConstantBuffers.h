#pragma once

#include <DirectXMath.h>

struct alignas(16) CTransformBuffer
{
        DirectX::XMMATRIX ViewProjection;
        DirectX::XMMATRIX World;
        DirectX::XMMATRIX Projection;
};

struct alignas(16) CSceneInfoBuffer
{
        DirectX::XMFLOAT3 eyePosition;
        float             time;
        DirectX::XMFLOAT3 directionalLightDirection;
        float              playerRadius;
        DirectX::XMFLOAT3A directionalLightColor;
		//pad
        DirectX::XMFLOAT3A ambientColor;
		//pad
};

struct alignas(16) CAnimationBuffer
{
        DirectX::XMMATRIX jointTransforms[64];
};

struct alignas(16) CScreenSpaceBuffer
{
        DirectX::XMMATRIX invProj;
        DirectX::XMMATRIX invView;
        DirectX::XMFLOAT3 playerPosition;
        float             time;
};