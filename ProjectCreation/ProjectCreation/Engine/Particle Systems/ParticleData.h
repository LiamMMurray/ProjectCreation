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
                float             lifeSpan; // life time
                bool              active;
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT4 initialColor;
                DirectX::XMFLOAT4 finalColor;
                DirectX::XMFLOAT2 uv;
                DirectX::XMFLOAT3 minVelocity;
                DirectX::XMFLOAT3 maxVelocity;
                DirectX::XMFLOAT2 scale; // min sclae and max scale
                DirectX::XMFLOAT3 acceleration;
                int               index; // type of particles;
				//defult values for emitters
				void Zero(FEmitterGPU& emitterGPU); //Set all values to ZERO
                void FloatParticle(FEmitterGPU&      emitterGPU,
                                   DirectX::XMFLOAT4 position,
                                   DirectX::XMFLOAT4 initialColor,
                                   DirectX::XMFLOAT4 finalColor); // Floating particles
        };

        struct FParticleGPU
        {
                DirectX::XMFLOAT4 position;
                DirectX::XMFLOAT4 prevPos;
                DirectX::XMFLOAT4 color;
                DirectX::XMFLOAT3 velocity;
                DirectX::XMFLOAT2 texture;
                float             time = 0.0f;
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