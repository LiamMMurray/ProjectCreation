#pragma once
#include "../../ECS/Component.h"
#include "../MathLibrary/Quaternion.h"
#include "ParticleData.h"
class EmitterComponent : public Component<EmitterComponent>
{
    public:
        ParticleData::FEmitterGPU EmitterData;
        DirectX::XMVECTOR         offset       = DirectX::XMVectorZero();
        float                     spawnRate    = 1000.5f;
        float                     desiredCount = 1.0f;
        int                       maxCount     = ParticleData::gMaxParticleCount;
        DirectX::XMVECTOR         rotationAxis = VectorConstants::Up;
        float                     rotationRate = DirectX::XM_PIDIV2;
        bool                      rotate       = false;
        bool                      active       = true;
        // defult values for emitters
        void Zero(); // Set all values to ZERO
        void FloatParticle(DirectX::XMFLOAT3 minOffset,
                           DirectX::XMFLOAT3 maxOffset,
                           DirectX::XMFLOAT4 initialColor,
                           DirectX::XMFLOAT4 finalColor,
                           DirectX::XMFLOAT4 lifeSpan); // Floating particles
        void ParticleswithGravity(DirectX::XMFLOAT3 minOffset,
                                  DirectX::XMFLOAT3 maxOffset,
                                  DirectX::XMFLOAT4 initialColor,
                                  DirectX::XMFLOAT4 finalColor,
                                  DirectX::XMFLOAT4 lifeSpan); // gravity is defult -9.8f
        void ParticleFloatUp(DirectX::XMFLOAT3 minOffset,
                             DirectX::XMFLOAT3 maxOffset,
                             DirectX::XMFLOAT4 initialColor,
                             DirectX::XMFLOAT4 finalColor,
                             DirectX::XMFLOAT4 lifeSpan);
};