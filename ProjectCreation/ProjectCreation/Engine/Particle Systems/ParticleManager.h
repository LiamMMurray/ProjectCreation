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
        static ParticleManager* instance;
        RenderSystem*           m_RenderSystem;
        // Buffers
        ParticleBuffer m_ParticleBuffer;
        ParticleBuffer m_EmitterBuffer;
        ParticleBuffer m_SegmentBuffer;
        // Shader Handles
        ResourceHandle m_ComputeShaderHandle;
        ResourceHandle m_GeometryShaderHandle;
        ResourceHandle m_VertexShaderHandle;
        ResourceHandle m_PixelShaderHandle;
        ResourceHandle m_TextureHandle;

        HandleManager*                          m_HandleManager;
        ID3D11InputLayout*                      m_VertexInputLayout;
        ParticleData::FParticleGPU*             m_ParticleInfo;
        ParticleData::FEmitterGPU*              m_EnitterInfo;
        ParticleData::FEmitterCPU*              m_EmitterCpuInfo;
        std::vector<ParticleData::FEmitterGPU*> m_Emitters;
        ParticleData::FSegmentBuffer            m_SegmentInfo;
        void                                    UpdateResources(ID3D11Resource* resource);
        void                                    update(float deltaTime);
        void                                    init();
        void                                    shutdown();

        void ParticleBufferInit(ID3D11Device1*                device1,
                                ParticleData::FParticleGPU*   particleData,
                                ParticleData::FEmitterGPU*    emitterData,
                                ParticleData::FSegmentBuffer* segmentData,
                                unsigned int                  numParticles);
        void ParticleBufferShutdown(ParticleBuffer* buffer);

    public:
        EntityHandle                   CreateEmitter(ParticleData::FEmitterGPU& emitter);
        void                           AddEmitter(ParticleData::FEmitterGPU& emitter);
        void                           SetParticleInfo(ParticleData::FParticleGPU* particleInfo);
        static void                    Initialize();
        static void                    Update(float deltaTime);
        static void                    Shutdown();
        static inline ParticleManager* Get()
        {
                return instance;
        }
};