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
                int   currentParticleCount;
                float accumulatedTime;
        };

        struct FParticleGPU
        {
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT4 color;
                DirectX::XMFLOAT3 velocity;
                DirectX::XMFLOAT2 texture;
                float             time;
                bool              active;
                int               index;
        };


} // namespace ParticleData