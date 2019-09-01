#pragma once
#include <ECS.h>
#include <D3DNativeTypes.h>
#include "../ResourceManager/IResource.h"
#include "ParticleBufferSetup.h"
#include "ParticleData.h"
#include "Pools.h"
#include "EmitterComponent.h"
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
        ParticleBuffer m_SegmentBufferGPU;
        // Shader Handles
        ResourceHandle m_SimulationComputeShaderHandle;
        ResourceHandle m_EmittionComputeShaderHandle;
        ResourceHandle m_GeometryShaderHandle;
        ResourceHandle m_VertexShaderHandle;
        ResourceHandle m_PixelShaderHandle;
        ResourceHandle m_TextureHandle;

        ParticleData::FSegmentBuffer            m_SegmentBufferCPU;
        HandleManager*                          m_HandleManager;
        ID3D11InputLayout*                      m_VertexInputLayout;
        ParticleData::FParticleGPU*             m_ParticleInfo;
        ParticleData::FEmitterGPU               m_EmittersCPU[ParticleData::gMaxEmitterCount];
        std::vector<EmitterComponent*>          m_Emitters;
        int                                     m_PreviousEmitterCount = 0;
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
        void                           AddEmitter(EmitterComponent* emitter);
        void                           SetParticleInfo(ParticleData::FParticleGPU* particleInfo);
        static void                    Initialize();
        static void                    Update(float deltaTime);
        static void                    Shutdown();
        static inline ParticleManager* Get()
        {
                return instance;
        }
};