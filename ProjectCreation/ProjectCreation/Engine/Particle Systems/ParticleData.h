#pragma once
#include <DirectXMath.h>
#include <vector>
#include "../../ECS/Component.h"
#include "Pools.h"
namespace ParticleData
{
        /*   struct FParticleType
           {
                   DirectX::XMFLOAT4 position;
                   DirectX::XMFLOAT4 prev_position;
                   DirectX::XMFLOAT4 color;
                   bool              active;
           };
   */
        /* struct FParticleBuffer
         {
                 DirectX::XMFLOAT4 position;
                 DirectX::XMFLOAT2 texture;
                 DirectX::XMFLOAT4 color;
         };*/

        struct FEmitter
        {
                DirectX::XMFLOAT4 spawnPosition;
                DirectX::XMFLOAT4 spawnColor;
                DirectX::XMFLOAT3 velocity;
                DirectX::XMFLOAT2 texture;
                float             time;
                FEmitter();
                FEmitter(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT2 uv, float time);
        };

        struct FParticleSystemElement
        {
                std::vector<FEmitter> emitters;
                int                   particleCount;
                int                   maxParticles;
        };

        class ParticleComponent : public Component<ParticleComponent>
        {
            public:
                FEmitter particleType;
        };
} // namespace ParticleData