#pragma once
#include "../../ECS/Component.h"
#include "ParticleData.h"
class EmitterComponent : public Component<EmitterComponent>
{
    public:
        ParticleData::FEmitterGPU EmitterData;
        DirectX::XMVECTOR         offset       = DirectX::XMVectorZero();
        float                     spawnRate    = 1000.5f;
        float                     desiredCount = 1.0f;
        int                       maxCount     = 2048;
        // defult values for emitters
        void Zero(ParticleData::FEmitterGPU& emitterGPU); // Set all values to ZERO
        void FloatParticle(ParticleData::FEmitterGPU& emitterGPU,
                           DirectX::XMFLOAT4 initPosition,
                           DirectX::XMFLOAT4 endPosition,
                           DirectX::XMFLOAT4 initialColor,
                           DirectX::XMFLOAT4 finalColor,
                           DirectX::XMFLOAT3          lifeSpan); // Floating particles
};