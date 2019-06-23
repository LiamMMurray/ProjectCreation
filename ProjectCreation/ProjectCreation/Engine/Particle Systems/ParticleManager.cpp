#include "ParticleManager.h"
#include <d3d11_1.h>
#include "../../Engine/MathLibrary/MathLibrary.h"
#include "../../Rendering/RenderingSystem.h"
#include "../../Rendering/Vertex.h"
#include "../../Utility/Macros/DirectXMacros.h"
#include "../GEngine.h"
#include "../ResourceManager/ComputeShader.h"
#include "../ResourceManager/GeometryShader.h"
#include "../ResourceManager/PixelShader.h"
#include "../ResourceManager/VertexShader.h"
#include "EmitterComponent.h"
#include "ParticleBufferSetup.h"

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
        {
                for (auto emitterComponent : GEngine::Get()->GetHandleManager()->GetActiveComponents<EmitterComponent>())
                {
                        int x = 5;
                }
        }
        // Send data to gpu
        ComputeShader*  computeShader  = RESOURCE_MANAGER->GetResource<ComputeShader>(m_ComputeShaderHandle);
        VertexShader*   vertexShader   = RESOURCE_MANAGER->GetResource<VertexShader>(m_VertexShaderHandle);
        PixelShader*    pixelShader    = RESOURCE_MANAGER->GetResource<PixelShader>(m_PixelShaderHandle);
        GeometryShader* geometryShader = RESOURCE_MANAGER->GetResource<GeometryShader>(m_GeometryShaderHandle);

        ID3D11ShaderResourceView* null[]{nullptr};
        m_RenderSystem->m_Context->VSSetShaderResources(0, 1, null);
        // Set Compute Shaders
        m_RenderSystem->m_Context->CSSetShader(computeShader->m_ComputerShader, nullptr, 0);
        m_RenderSystem->m_Context->CSSetUnorderedAccessViews(0, 1, &m_ParticleBuffer.m_UAV, 0);
        m_RenderSystem->m_Context->CSSetShaderResources(0,1,&m_EmitterBuffer.m_StructuredView);
        // dispatch before setting UAV to null
        m_RenderSystem->m_Context->Dispatch(1, 1, 1);

        // set compute shader's UAV to null after dispatch
        ID3D11UnorderedAccessView* nullUAV = NULL;

        m_RenderSystem->m_Context->CSSetUnorderedAccessViews(
            0, 1, &nullUAV, 0); // Set UAV to null before have other shader able to get data from compute shader
        m_RenderSystem->m_Context->VSSetShaderResources(1, 0, &m_ParticleBuffer.m_StructuredView);

        m_RenderSystem->m_Context->GSSetShader(geometryShader->m_GeometryShader, 0, 0);
        m_RenderSystem->m_Context->GSSetShaderResources(0, 0, &m_ParticleBuffer.m_StructuredView);

        // draw call;


        // UpdateConstantBuffer(
        //      m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP], &m_ConstantBuffer_MVP,
        //      sizeof(m_ConstantBuffer_MVP));

        // reset srv null for geometry shader

        ID3D11ShaderResourceView* nullSRV = nullptr;
        m_RenderSystem->m_Context->GSSetShaderResources(0, 0, &nullSRV);

        m_RenderSystem->m_Swapchain->Present(1, 0);
}

void ParticleManager::init()
{
        // init
        m_RenderSystem = SYSTEM_MANAGER->GetSystem<RenderSystem>();
        ParticleBufferInit(
            &m_ParticleBuffer, &m_EmitterBuffer, m_RenderSystem->GetDevice(), nullptr, nullptr, gMaxParticleCount);
        // Load Computer Shader
        m_ComputeShaderHandle  = RESOURCE_MANAGER->LoadComputeShader("ComputeShaderTesting");
        m_PixelShaderHandle    = RESOURCE_MANAGER->LoadPixelShader("PurePixelShader");
        m_VertexShaderHandle   = RESOURCE_MANAGER->LoadVertexShader("PureVertexShader");
        m_GeometryShaderHandle = RESOURCE_MANAGER->LoadGeometryShader("GeometryShaderTesting");
}

void ParticleManager::shutdown()
{
        ParticleBufferShutdown(&m_ParticleBuffer);
}

void ParticleManager::ParticleBufferInit(ParticleBuffer*             particlebuffer,
                                         ParticleBuffer*             emitterBuffer,
                                         ID3D11Device1*              device1,
                                         ParticleData::FParticleGPU* particleData,
                                         ParticleData::FEmitterGPU*  emitterData,
                                         unsigned int                numParticles)
{

        // Set up gpu particle information
        CD3D11_BUFFER_DESC sbDesc;
        sbDesc.BindFlags           = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        sbDesc.CPUAccessFlags      = 0;
        sbDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        sbDesc.StructureByteStride = sizeof(FParticleGPU);
        sbDesc.ByteWidth           = sizeof(FParticleGPU) * numParticles * 1;
        sbDesc.Usage               = D3D11_USAGE_DEFAULT;

        D3D11_SUBRESOURCE_DATA rwData;
        rwData.pSysMem          = particleData; // not sure should pass in what type of data
        rwData.SysMemPitch      = 0;
        rwData.SysMemSlicePitch = 0;
        device1->CreateBuffer(&sbDesc, &rwData, &particlebuffer->m_StructuredBuffer);

        D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
        sbUAVDesc.Buffer.FirstElement = 0;
        sbUAVDesc.Buffer.Flags        = 0;
        sbUAVDesc.Buffer.NumElements  = numParticles * 1;
        sbUAVDesc.Format              = DXGI_FORMAT_UNKNOWN;
        sbUAVDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
        // HREFTYPE hr = m_Device->CreateUnorderedAccessView(m_StructuredBuffer.Get(), &sbUAVDesc, m_UAV.GetAddressOf());
        HREFTYPE hr =
            device1->CreateUnorderedAccessView(particlebuffer->m_StructuredBuffer,
                                               &sbUAVDesc,
                                               &particlebuffer->m_UAV); // if passing the &sbUAVDesc as nullptr, get the
                                                                        // infprmation from
                                                                        // the
                                                                        // buffer and creates the viewDesc from it


        D3D11_SHADER_RESOURCE_VIEW_DESC sbSRVDesc;
        sbSRVDesc.Buffer.ElementOffset = 0;
        sbSRVDesc.Buffer.ElementWidth  = sizeof(FParticleGPU);
        sbSRVDesc.Buffer.FirstElement  = 0;
        sbSRVDesc.Buffer.NumElements   = numParticles * 1;
        sbSRVDesc.Format               = DXGI_FORMAT_UNKNOWN;
        sbSRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
        hr                             = device1->CreateShaderResourceView(
            particlebuffer->m_StructuredBuffer, &sbSRVDesc, &particlebuffer->m_StructuredView);

        // Zero memeory the data type
        ZeroMemory(&sbDesc, sizeof(CD3D11_BUFFER_DESC));
        ZeroMemory(&rwData, sizeof(D3D11_SUBRESOURCE_DATA));
        ZeroMemory(&sbUAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
        ZeroMemory(&sbSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

        // Set up emitter gpu information

        // CD3D11_BUFFER_DESC
        sbDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
        sbDesc.CPUAccessFlags      = 0;
        sbDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        sbDesc.StructureByteStride = sizeof(FEmitterGPU);
        sbDesc.ByteWidth           = sizeof(FEmitterGPU) * numParticles * 1;
        sbDesc.Usage               = D3D11_USAGE_DEFAULT;
        // D3D11_SUBRESOURCE_DATA
        // rwData.pSysMem          = particleData;
        rwData.SysMemPitch      = 0;
        rwData.SysMemSlicePitch = 0;
        hr = device1->CreateBuffer(&sbDesc, &rwData, &emitterBuffer->m_StructuredBuffer);

        // D3D11_SHADER_RESOURCE_VIEW_DESC
        sbSRVDesc.Buffer.ElementOffset = 0;
        sbSRVDesc.Buffer.ElementWidth  = sizeof(FEmitterGPU);
        sbSRVDesc.Buffer.FirstElement  = 0;
        sbSRVDesc.Buffer.NumElements   = numParticles * 1;
        sbSRVDesc.Format               = DXGI_FORMAT_UNKNOWN;
        sbSRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
        hr = device1->CreateShaderResourceView(emitterBuffer->m_StructuredBuffer, &sbSRVDesc, &emitterBuffer->m_StructuredView);
}

void ParticleManager::ParticleBufferShutdown(ParticleBuffer* buffer)
{
        SAFE_RELEASE(buffer->m_StructuredBuffer);
        SAFE_RELEASE(buffer->m_UAV);
        SAFE_RELEASE(buffer->m_StructuredView);
}

EntityHandle ParticleManager::CreateEmitter(ParticleData::FEmitterCPU& emitter)
{
        EntityHandle      eHandle    = GEngine::Get()->GetHandleManager()->CreateEntity();
        ComponentHandle   cHandle    = GEngine::Get()->GetHandleManager()->AddComponent<EmitterComponent>(eHandle);
        EmitterComponent* eComponent = GEngine::Get()->GetHandleManager()->GetComponent<EmitterComponent>(cHandle);
        eComponent->mEmitterData     = emitter;

        return eHandle;
}

void ParticleManager::SetParticleInfo(ParticleData::FParticleGPU* particleInfo)
{
        m_ParticleInfo = particleInfo;
}


void ParticleManager::Initialize()
{
        instance = new ParticleManager;
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
