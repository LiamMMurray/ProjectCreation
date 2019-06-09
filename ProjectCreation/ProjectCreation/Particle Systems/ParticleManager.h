#pragma once
#include"ParticleInfo.h"
#include"../ECS/ECS.h"
class ParticleManager
{
    public:
        void CreateParticle(ParticleInfo::FParticleType& particletype, EntityHandle& entityHandle, ComponentHandle& componentHandle);
};