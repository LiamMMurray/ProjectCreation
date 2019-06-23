#pragma once
#include "../../ECS/ECS.h"
#include "../../Utility/ForwardDeclarations/D3DNativeTypes.h"
#include "../ResourceManager/IResource.h"
#include "ParticleBufferSetup.h"
#include "ParticleData.h"
#include "Pools.h"
class RenderSystem;
class ComponentManager;
class ParticleManager
{
    private:
        static constexpr unsigned int gMaxParticleCount = 100'000;

        static ParticleManager*    instance;
        RenderSystem*              m_RenderSystem;
        ParticleBuffer             m_ParticleBuffer;
        ParticleBuffer             m_EmitterBuffer;
        ResourceHandle             m_ComputeShaderHandle;
        ResourceHandle             m_GeometryShaderHandle;
        ResourceHandle             m_VertexShaderHandle;
        ResourceHandle             m_PixelShaderHandle;
        ParticleData::FParticleGPU* m_ParticleInfo;
        ParticleData::FEmitterGPU*  m_EnitterInfo;
        void                       UpdateResources(ID3D11Resource* resource);
        void                       update(float deltaTime);
        void                       init();
        void                       shutdown();

        void ParticleBufferInit(ParticleBuffer*             particlesbuffer,
			ParticleBuffer* emitterBuffer,
                                ID3D11Device1*              device1,
                                ParticleData::FParticleGPU* particleData,
                                ParticleData::FEmitterGPU*  emitterData,
                                unsigned int                         numParticles);
        void ParticleBufferShutdown(ParticleBuffer* buffer);
    public:
        EntityHandle                   CreateEmitter(ParticleData::FEmitterCPU& emitter);
        void                           SetParticleInfo(ParticleData::FParticleGPU* particleInfo);
        static void                    Initialize();
        static void                    Update(float deltaTime);
        static void                    Shutdown();
        static inline ParticleManager* Get()
        {
                return instance;
        }
};