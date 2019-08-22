#pragma once
#include <DirectXMath.h>
#include <vector>
#include "../../ECS/Component.h"
#include "Pools.h"
#define ALIGN_TO_VEL (1 << 0)
namespace ParticleData
{

        static constexpr unsigned int gMaxParticleCount = 2 << 18;
        static constexpr unsigned int gMaxEmitterCount  = 2 << 10;

        struct FEmitterGPU
        {
                DirectX::XMFLOAT4 lifeSpan; // x is life time, y is variance, z,w is fade in ad out
                unsigned int      flags = 0; // defult collision, ==1 is no collision on particles
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
                unsigned int      index; // tpye of particles
                unsigned int      textureIndex;
        };

        struct FParticleGPU
        {
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT4 prevPos;
                DirectX::XMFLOAT4 initialColor;
                DirectX::XMFLOAT4 finalColor;
                DirectX::XMFLOAT3 velocity;
                float             time     = 0.0f;
                DirectX::XMFLOAT3 lifeSpan = {1.0f, 0.0f, 0.0f};
                unsigned int      flags;
                DirectX::XMFLOAT2 scale;
                DirectX::XMFLOAT3 acceleration;
                unsigned int      textureIndex = 0;
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