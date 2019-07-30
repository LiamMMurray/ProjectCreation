#pragma once
#include <DirectXMath.h>
#include <vector>
#include "../../ECS/Component.h"
#include "Pools.h"
namespace ParticleData
{

        static constexpr unsigned int gMaxParticleCount = 2 << 18;
        static constexpr unsigned int gMaxEmitterCount  = 2 << 10;

        struct FEmitterGPU
        {
                DirectX::XMFLOAT4 lifeSpan; // x is life time, y is variance, z,w is fade in ad out
                int               flags;
                DirectX::XMFLOAT3 emitterPosition;
                DirectX::XMFLOAT3 minOffset;
                DirectX::XMFLOAT3 maxOffset;
                DirectX::XMFLOAT4 initialColor;
                DirectX::XMFLOAT4 finalColor;
                DirectX::XMFLOAT2 uv;
                DirectX::XMFLOAT3 minInitialVelocity;
                DirectX::XMFLOAT3 maxInitialVelocity;
                DirectX::XMFLOAT2 particleScale;
                DirectX::XMFLOAT3 acceleration;
                int index; // tpye of particles
        };

        struct FParticleGPU
        {
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT4 prevPos;
                DirectX::XMFLOAT4 color;
                DirectX::XMFLOAT3 velocity;
                DirectX::XMFLOAT2 texture;
                float             time     = 0.0f;
                float             lifeSpan = 1.0f;
                int               active;
                float             scale;
                DirectX::XMFLOAT3 acceleration;
        };


        struct FTextureSetting
        {
                int index;
                int col;
                int row;
        };

        struct FSegmentBuffer
        {
                int desiredCount[gMaxEmitterCount];
        };
}; // namespace ParticleData