#include "ParticleManager.h"
#include <d3d11_1.h>
#include "../../Rendering/RenderingSystem.h"
#include "../GEngine.h"
#include "ParticleBufferSetup.h"

#include "../../Engine/MathLibrary/MathLibrary.h"
#include "../../Rendering/Vertex.h"
#include "../ResourceManager/ComputeShader.h"
#include "../ResourceManager/GeometryShader.h"
#include "../ResourceManager/PixelShader.h"
#include "../ResourceManager/VertexShader.h"
#include "EmitterComponent.h"

using namespace ParticleData;
using namespace DirectX;
using namespace Pools;

ParticleManager* ParticleManager::instance;

void ParticleManager::UpdateResources(ID3D11Resource* resource)
{
        D3D11_MAPPED_SUBRESOURCE mappedResource{};
        m_RenderSystem->m_Context->Map(resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        // memcpy(mappedResource.pData, buffer->cpu, buffer->gpu.size);
        m_RenderSystem->m_Context->Unmap(resource, 0);
}

void ParticleManager::update(float deltaTime)
{
        // update emitter container
        // Build update list
        if (m_ComponentManager->ComponentsExist<EmitterComponent>())
        {
                auto itrEmitter = m_ComponentManager->GetActiveComponents<EmitterComponent>();
                for (auto itr = itrEmitter.begin(); itr != itrEmitter.end(); ++itr)
                {
                        EmitterComponent* emitterComponent = (EmitterComponent*)itr.data();
                        int               x                = 5;
                }
        }
        // Send data to gpu
        ComputeShader*  computeShader = RESOURCE_MANAGER->GetResource<ComputeShader>(m_ComputeShaderHandle);
        VertexShader* vertexShader  = RESOURCE_MANAGER->GetResource<VertexShader>(m_VertexShaderHandle);
        PixelShader*    pixelShader   = RESOURCE_MANAGER->GetResource<PixelShader>(m_PixelShaderHandle);
        GeometryShader* geometryShader = RESOURCE_MANAGER->GetResource<GeometryShader>(m_GeometryShaderHandle);
        m_BufferData->ComputeShaderSetup(m_RenderSystem->m_Device, &m_ParticleInfo, 10);

        ID3D11ShaderResourceView* null[]{nullptr};
        m_RenderSystem->m_Context->VSSetShaderResources(0, 1, null);
        // Set Compute Shaders
        m_RenderSystem->m_Context->CSSetShader(computeShader->m_ComputerShader, nullptr, 0);
        m_RenderSystem->m_Context->CSSetUnorderedAccessViews(0, 1, &m_BufferData->m_UAV, 0);
        m_RenderSystem->m_Context->Dispatch(1, 1, 1);

        ID3D11UnorderedAccessView* nullUAV = NULL;

        m_RenderSystem->m_Context->CSSetUnorderedAccessViews(
            0, 1, &nullUAV, 0); // Set UAV to null before have other shader able to get data from compute shader
        m_RenderSystem->m_Context->VSSetShaderResources(1, 0, &m_BufferData->m_StructuredView);
}

void ParticleManager::init()
{
        // init
        m_ComponentManager = COMPONENT_MANAGER;
        m_EntityManager    = ENTITY_MANAGER;
        m_RenderSystem     = SYSTEM_MANAGER->GetSystem<RenderSystem>();
        m_BufferData       = BufferSetup::GetBufferData();
        // Load Computer Shader
        m_ComputeShaderHandle  = RESOURCE_MANAGER->LoadComputeShader("ComputeShaderTesting");
        m_PixelShaderHandle    = RESOURCE_MANAGER->LoadPixelShader("");
        m_VertexShaderHandle   = RESOURCE_MANAGER->LoadVertexShader("");
        m_GeometryShaderHandle = RESOURCE_MANAGER->LoadGeometryShader("");
}

void ParticleManager::shutdown()
{
        m_BufferData->ShutdownBuffer();
}

EntityHandle ParticleManager::CreateEmitter(ParticleData::FEmitterCPU& emitter)
{
        EntityHandle      eHandle    = m_EntityManager->CreateEntity<EntityHandle>();
        ComponentHandle   cHandle    = m_ComponentManager->AddComponent<EmitterComponent>(eHandle);
        EmitterComponent* eComponent = m_ComponentManager->GetComponent<EmitterComponent>(cHandle);
        eComponent->mEmitterData     = emitter;

        return eHandle;
}

void ParticleManager::SetParticleInfo(ParticleData::FParticleGPU& particleInfo)
{
        m_ParticleInfo = particleInfo;
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
