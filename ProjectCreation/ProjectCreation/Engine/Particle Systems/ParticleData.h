#pragma once
#include <DirectXMath.h>
#include <vector>
#include "../../ECS/Component.h"
#include "Pools.h"
namespace ParticleData
{
        /*  struct FParticleType
          {
                  DirectX::XMFLOAT4 position;
                  DirectX::XMFLOAT4 prev_position;
                  DirectX::XMFLOAT4 color;
                  DirectX::XMFLOAT3 velocity;
          };*/

        static constexpr unsigned int gMaxParticleCount = 2 << 16;
        static constexpr unsigned int gMaxEmitterCount  = 2 << 10;

        struct FEmitterCPU
        {
                bool              active;
                int               maxParticles;
                DirectX::XMFLOAT4 spawnPosition;
                DirectX::XMFLOAT4 spawnColor;
                DirectX::XMFLOAT3 velocity;
                DirectX::XMFLOAT2 texture;
                float             time;
                FEmitterCPU();
                FEmitterCPU(DirectX::XMFLOAT4 pos,
                            DirectX::XMFLOAT4 color,
                            DirectX::XMFLOAT3 vel,
                            DirectX::XMFLOAT2 uv,
                            float             t);
        };


        struct FEmitterGPU
        {
                int               currentParticleCount;
                float             accumulatedTime; // life time
                bool              active;
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT4 color;
                DirectX::XMFLOAT2 uv;
                DirectX::XMFLOAT3 minVelocity;
                DirectX::XMFLOAT3 maxVelocity;
        };

        struct FParticleGPU
        {
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT4 prevPos;
                DirectX::XMFLOAT4 color;
                DirectX::XMFLOAT3 velocity;
                DirectX::XMFLOAT2 texture;
                float             time = 0.0f;
                bool              active;
                int               index;
        };

        struct FSegmentBuffer
        {
                int index[gMaxEmitterCount];
        };


}; // namespace ParticleData