#pragma once
#include "../../ECS/ECS.h"
#include "ParticleData.h"
#include"../ResourceManager/IResource.h"
class RenderSystem;
class BufferSetup;
class ParticleManager
{
    private:
        ParticleManager();

        static ParticleManager* instance;
        RenderSystem*           m_RenderSystem;
        BufferSetup*            m_BufferData;
        ResourceHandle          m_rHandle;
        void                    update(float deltaTime);
        void                    init();
        void                    shutdown();

    public:
        static void CreateParticle(const ParticleData::FEmitter& particletype,
                                   EntityHandle                       entityHandle,
                                   ComponentHandle*                   componentHandle = nullptr);

        static void Initialize();
        static void Update(float deltaTime);
        static void Shutdown();

        static inline ParticleManager* Get()
        {
                return instance;
        }
};