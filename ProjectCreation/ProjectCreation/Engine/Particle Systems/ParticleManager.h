#pragma once
#include "../../ECS/ECS.h"
#include "../ResourceManager/IResource.h"
#include"../../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "ParticleData.h"
#include "Pools.h"
class RenderSystem;
class BufferSetup;
class ComponentManager;
class ParticleManager
{
    private:
        ParticleManager();

        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;

        static ParticleManager* instance;
        RenderSystem*           m_RenderSystem;
        BufferSetup*            m_BufferData;
        ResourceHandle          m_ComputeShaderHandle;
        ResourceHandle          m_GeometryShaderHandle;
        ResourceHandle          m_VertexShaderHandle;
        ResourceHandle          m_PixelShaderHandle;
        ParticleData::FParticleGPU m_ParticleInfo;
        void                    UpdateResources(ID3D11Resource* resource);
        void                    update(float deltaTime);
        void                    init();
        void                    shutdown();

    public:
        EntityHandle                   CreateEmitter(ParticleData::FEmitterCPU & emitter);
        void                           SetParticleInfo(ParticleData::FParticleGPU& particleInfo);
        static void                    Initialize();
        static void                    Update(float deltaTime);
        static void                    Shutdown();
        static inline ParticleManager* Get()
        {
                return instance;
        }
};