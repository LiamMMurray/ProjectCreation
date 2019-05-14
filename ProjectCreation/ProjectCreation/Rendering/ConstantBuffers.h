#pragma once

#include <DirectXMath.h>

struct CBuffer
{
        virtual ~CBuffer() = default;
};

struct CTransformBuffer : public CBuffer
{
        DirectX::XMMATRIX ViewProjection;
        DirectX::XMMATRIX World;

		virtual ~CTransformBuffer() override = default;
};