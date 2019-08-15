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
        float             playerRadius;
        DirectX::XMFLOAT3 directionalLightColor;
        float             aspectRatio;
        DirectX::XMFLOAT3 ambientColor;
        float             deltaTime;
        DirectX::XMFLOAT2 screenDimensions;
        float             scale;
        float             _InstanceReveal;
        DirectX::XMFLOAT3 worldOffsetDelta;
        float             otherPadding;
};

struct alignas(16) CAnimationBuffer
{
        DirectX::XMMATRIX jointTransforms[64];
};

struct alignas(16) CScreenSpaceBuffer
{
        DirectX::XMMATRIX invProj;
        DirectX::XMMATRIX invView;
        DirectX::XMMATRIX Proj;
        DirectX::XMFLOAT3 playerPosition;
        float             time;
        DirectX::XMFLOAT3 selectionColor;
        float             selectionAlpha;
};