#include "ParticleManager.h"
#include "../GEngine.h"
#include "../../Rendering/RenderingSystem.h"
#include"ParticleBufferSetup.h"
#include<d3d11_1.h>

#include "../ResourceManager/ComputeShader.h"
#include "../ResourceManager/GeometryShader.h"
#include "../ResourceManager/PixelShader.h"
#include "../ResourceManager/VertexShader.h"
using namespace ParticleData;

ParticleManager* ParticleManager::instance;

void ParticleManager::update(float deltaTime)
{}

void ParticleManager::init()
{
        m_RenderSystem = SYSTEM_MANAGER->GetSystem<RenderSystem>();
        m_BufferData   = BufferSetup::GetBufferData();
        m_rHandle     = RESOURCE_MANAGER->LoadComputeShader("ComputeShaderTesting");
        ComputeShader* computeShader = RESOURCE_MANAGER->GetResource<ComputeShader>(m_rHandle);
        m_BufferData->ComputeShaderSetup(m_RenderSystem->m_Device);
        m_RenderSystem->m_Context->CSSetShader(computeShader->m_ComputerShader, nullptr, 0);
        m_RenderSystem->m_Context->CSSetUnorderedAccessViews(0, 1, &m_BufferData->m_UAV, 0);

}

void ParticleManager::shutdown()
{}

void ParticleManager::CreateParticle(const ParticleData::FEmitter& particletype,
                                     EntityHandle                       entityHandle,
                                     ComponentHandle*                   componentHandle)
{
        ComponentHandle pHandle = COMPONENT_MANAGER->AddComponent<ParticleComponent>(entityHandle);

        ParticleComponent* pComponent = COMPONENT_MANAGER->GetComponent<ParticleComponent>(pHandle);
        pComponent->particleType      = particletype;
        if (componentHandle)
                *componentHandle = pHandle;
}

void ParticleManager::Initialize()
{
        instance = new ParticleManager();
        instance->init();
}

void ParticleManager::Update(float deltaTime)
{
        instance->update(deltaTime);
}

void ParticleManager::Shutdown()
{
        instance->shutdown();
        delete instance;
}
