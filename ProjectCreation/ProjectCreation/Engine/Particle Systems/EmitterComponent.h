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
};