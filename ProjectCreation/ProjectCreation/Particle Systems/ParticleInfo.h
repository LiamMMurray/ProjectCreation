#pragma once
#include <vector>
#include "../Engine/MathLibrary/MathLibrary.h"
#include "Pools.h"
namespace ParticleInfo
{
        struct FParticleType
        {
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT4 prev_position;
                DirectX::XMFLOAT4 color;
                float             velocity;
                bool              active;
        };

        struct FParticleVertex
        {
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT2 texture;
                DirectX::XMFLOAT4 color;
        };

        struct FEmitter
        {
                DirectX::XMFLOAT4                  spawnPosition;
                DirectX::XMFLOAT4                  spawnColor;
                Pools::sorted_pool_t<int16_t, 256> indices;
        };

        struct FParticleSystemElement
        {
                std::vector<FEmitter> emitters;
                int                   particleCount;
                int                   maxParticles;

        };

} // namespace ParticleInfo