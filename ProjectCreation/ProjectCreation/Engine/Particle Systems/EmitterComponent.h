#pragma once

#include "../../ECS/Component.h"
#include "ParticleData.h"
class EmitterComponent : public Component<EmitterComponent>
{
    public:
        ParticleData::FEmitterCPU mEmitterData;
};