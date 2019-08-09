#include "TerrainManager.h"
#include <d3d11.h>
#include <d3d11_1.h>
#include < Directxpackedvector.h >
#include "../..//Engine/ResourceManager/StaticMesh.h"
#include "../..//FileIO/FileIO.h"
#include "../..//Utility/StringUtility.h"
#include "../../Engine/ResourceManager/ComputeShader.h"
#include "..//..//Engine/Controller/ControllerSystem.h"
#include "..//..//Engine/CoreInput/CoreInput.h"
#include "..//..//Engine/GEngine.h"
#include "..//..//Engine/GenericComponents/TransformComponent.h"
#include "..//..//Engine/GenericComponents/TransformSystem.h"
#include "..//..//Engine/ResourceManager/Material.h"
#include "..//..//Utility/Macros/DirectXMacros.h"
#include "..//DebugRender/debug_renderer.h"
#include "..//RenderingSystem.h"

TerrainManager* TerrainManager::instance;

void TerrainManager::_initialize(RenderSystem* rs)
{
        renderSystem = rs;

        // Terrain shaders
        {
                FileIO::FShaderData vertexShaderData;
                FileIO::LoadShaderDataFromFile("Terrain", "_VS", &vertexShaderData);
                rs->GetDevice()->CreateVertexShader(
                    vertexShaderData.bytes.data(), vertexShaderData.bytes.size(), nullptr, &vertexShader);

                FileIO::FShaderData domainShaderData;
                FileIO::LoadShaderDataFromFile("Terrain", "_DS", &domainShaderData);
                rs->GetDevice()->CreateDomainShader(
                    domainShaderData.bytes.data(), domainShaderData.bytes.size(), nullptr, &domainShader);

                FileIO::FShaderData hullShaderData;
                FileIO::LoadShaderDataFromFile("Terrain", "_HS", &hullShaderData);
                rs->GetDevice()->CreateHullShader(
                    hullShaderData.bytes.data(), hullShaderData.bytes.size(), nullptr, &hullShader);

                FileIO::FShaderData pixelShaderData;
                FileIO::LoadShaderDataFromFile("Terrain", "_PS", &pixelShaderData);
                rs->GetDevice()->CreatePixelShader(
                    pixelShaderData.bytes.data(), pixelShaderData.bytes.size(), nullptr, &pixelShader);

                D3D11_INPUT_ELEMENT_DESC vLayout[] = {
                    {"POSITION",
                     0,
                     DXGI_FORMAT_R32G32B32_FLOAT,
                     0,
                     D3D11_APPEND_ALIGNED_ELEMENT,
                     D3D11_INPUT_PER_VERTEX_DATA,
                     0},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                };

                rs->GetDevice()->CreateInputLayout(
                    vLayout, 2, vertexShaderData.bytes.data(), vertexShaderData.bytes.size(), &inputLayout);
        }

        // Ocean shaders
        {
                FileIO::FShaderData pixelShaderData;
                FileIO::LoadShaderDataFromFile("Ocean", "_PS", &pixelShaderData);
                rs->GetDevice()->CreatePixelShader(
                    pixelShaderData.bytes.data(), pixelShaderData.bytes.size(), nullptr, &oceanPixelShader);

                FileIO::FShaderData domainShaderData;
                FileIO::LoadShaderDataFromFile("Ocean", "_DS", &domainShaderData);
                rs->GetDevice()->CreateDomainShader(
                    domainShaderData.bytes.data(), domainShaderData.bytes.size(), nullptr, &oceanDomainShader);
        }


        std::wstring str = L"../Assets/Textures/TerrainTest.dds";

        ID3D11Resource* resource;
        DirectX::CreateDDSTextureFromFile(renderSystem->GetDevice(), str.c_str(), &resource, &terrainSourceSRV);
        ID3D11Texture2D* texture = (ID3D11Texture2D*)resource;

        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);
        UINT texwidth         = desc.Width;
        UINT texheight        = desc.Height;
        textureDimensions     = texwidth;
        patchSquareDimensions = ((texwidth - 1) / patchCells) + 1;
        patchQuadCount        = (patchSquareDimensions - 1) * (patchSquareDimensions - 1);
        texture->Release();

        DirectX::CreateDDSTextureFromFile(renderSystem->GetDevice(), str.c_str(), &resource, &terrainMaskSRV);
        resource->Release();

        assert(texwidth == texheight);

        CreateVertexBuffer(&vertexBuffer, patchSquareDimensions, WaterLevel, 8000.0f);
        CreateIndexBuffer(&indexBuffer, patchSquareDimensions);

        {
                D3D11_BUFFER_DESC bd{};
                bd.Usage          = D3D11_USAGE_DYNAMIC;
                bd.ByteWidth      = sizeof(CTerrainInfoBuffer);
                bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
                bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                renderSystem->GetDevice()->CreateBuffer(&bd, nullptr, &terrainConstantBufferGPU);
        }


        terrainConstantBufferCPU.gTexelSize      = 1.0f / texwidth;
        terrainConstantBufferCPU.gTexScale       = 64.0f;
        terrainConstantBufferCPU.gWorldCellSpace = 8000.0f / texwidth;
        terrainConstantBufferCPU.gScale          = 8000.0f * scale;
        terrainConstantBufferCPU.gTerrainAlpha   = GEngine::Get()->m_TerrainAlpha;
        terrainConstantBufferCPU.gCellSizeWorld  = 8000.0f / (patchSquareDimensions - 1) * scale;


        {
                intermediateMipDimensions = texwidth / pow(2, intermediateMipLevel);

                D3D11_TEXTURE2D_DESC texd{};
                texd.ArraySize          = 1;
                texd.BindFlags          = 0;
                texd.MiscFlags          = 0;
                texd.Width              = intermediateMipDimensions;
                texd.Height             = intermediateMipDimensions;
                texd.Format             = desc.Format;
                texd.Usage              = D3D11_USAGE_STAGING;
                texd.CPUAccessFlags     = D3D11_CPU_ACCESS_READ;
                texd.SampleDesc.Count   = 1;
                texd.SampleDesc.Quality = 0;
                texd.MipLevels          = 1;

                stagingTextureCPUElementCount = texd.Width * texd.Height;
                terrainHeightArray            = DBG_NEW float[texd.Height * texd.Width]();

                D3D11_SUBRESOURCE_DATA data{};
                data.pSysMem     = (void*)terrainHeightArray;
                data.SysMemPitch = texd.Width;

                renderSystem->GetDevice()->CreateTexture2D(&texd, &data, &stagingTextureResource);
        }

        {
                desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
                desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
                desc.MipLevels = intermediateMipLevel + 1;
                renderSystem->GetDevice()->CreateTexture2D(&desc, nullptr, &terrainIntermediateTexture);

                renderSystem->GetDevice()->CreateRenderTargetView(
                    terrainIntermediateTexture, NULL, &terrainIntermediateRenderTarget);
                renderSystem->GetDevice()->CreateShaderResourceView(
                    terrainIntermediateTexture, nullptr, &terrainIntermediateSRV);
        }

        TerrainMatrix        = DirectX::XMMatrixScaling(scale, scale, scale);
        InverseTerrainMatrix = DirectX::XMMatrixInverse(nullptr, TerrainMatrix);

        mTextureHandles[0] = GEngine::Get()->GetResourceManager()->LoadTexture2D("Cliff01_Diffuse");
        mTextureHandles[1] = GEngine::Get()->GetResourceManager()->LoadTexture2D("Cliff01_Normal");
        mTextureHandles[2] = GEngine::Get()->GetResourceManager()->LoadTexture2D("Soil01_Diffuse");
        mTextureHandles[3] = GEngine::Get()->GetResourceManager()->LoadTexture2D("Soil01_Normal");
        mTextureHandles[4] = GEngine::Get()->GetResourceManager()->LoadTexture2D("Terrain_Roughness");
        mTextureHandles[5] = GEngine::Get()->GetResourceManager()->LoadTexture2D("WaterHigh_NM");

        if (true)
        {
                ID3D11Resource* sourceResource;
                terrainSourceSRV->GetResource(&sourceResource);
                sourceResource->Release();
                renderSystem->GetContext()->CopySubresourceRegion(
                    terrainIntermediateTexture, 0, 0, 0, 0, sourceResource, 0, nullptr);

                renderSystem->GetContext()->GenerateMips(terrainIntermediateSRV);
                renderSystem->GetContext()->CopySubresourceRegion(
                    stagingTextureResource, 0, 0, 0, 0, terrainIntermediateTexture, intermediateMipLevel, nullptr);

                D3D11_MAPPED_SUBRESOURCE mappedResource{};
                renderSystem->GetContext()->Map(stagingTextureResource, 0, D3D11_MAP_READ, 0, &mappedResource);

                BYTE* mappedData = reinterpret_cast<BYTE*>(mappedResource.pData);
                BYTE* buffer     = reinterpret_cast<BYTE*>(terrainHeightArray);
                for (UINT i = 0; i < intermediateMipDimensions; ++i)
                {

                        memcpy(buffer, mappedData, intermediateMipDimensions * sizeof(float));

                        mappedData += mappedResource.RowPitch;

                        buffer += intermediateMipDimensions * sizeof(float);
                }
                renderSystem->GetContext()->Unmap(stagingTextureResource, 0);
        }

        // Create instance data and buffers

        GenerateInstanceTransforms(m_InstanceTransforms);

        ResourceManager* resourceManager = GEngine::Get()->GetResourceManager();

        FInstanceRenderData renderTestData;
        renderTestData.instanceCount = gInstanceTransformsCount;
        renderTestData.instanceIndexList.resize(gInstanceTransformsCount);
        for (int i = 0; i < gInstanceTransformsCount; ++i)
        {
                renderTestData.instanceIndexList[i] = i;
        }
        renderTestData.mesh     = resourceManager->LoadStaticMesh("Sphere01");
        renderTestData.material = resourceManager->LoadMaterial("DefaultInstanced");


        m_UpdateInstancesComputeShader = resourceManager->LoadComputeShader("InstanceUpdate");

        using namespace DirectX;
        HRESULT hr = {};
        { // Create transform buffer
                for (unsigned int i = 0; i < gInstanceTransformsCount; ++i)
                {
                        m_InstanceMatrices[i] = XMMatrixTranspose(m_InstanceTransforms[i].CreateMatrix());
                }

                // renderSystem->UpdateConstantBuffer(
                //    instanceBuffer, m_InstanceMatrices, sizeof(XMMATRIX) * gInstanceTransformsCount);

                D3D11_BUFFER_DESC                sbDesc{};
                D3D11_SUBRESOURCE_DATA           rwData{};
                D3D11_SHADER_RESOURCE_VIEW_DESC  srvDesc{};
                D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc{};
                ID3D11Buffer*                    tempBuffer;
                // CD3D11_BUFFER_DESC
                sbDesc.BindFlags           = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
                sbDesc.CPUAccessFlags      = 0;
                sbDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                sbDesc.StructureByteStride = sizeof(XMMATRIX);
                sbDesc.ByteWidth           = sizeof(XMMATRIX) * gInstanceTransformsCount;
                sbDesc.Usage               = D3D11_USAGE_DEFAULT;
                // D3D11_SUBRESOURCE_DATA
                rwData.pSysMem = m_InstanceMatrices;


                hr |= renderSystem->m_Device->CreateBuffer(&sbDesc, &rwData, &tempBuffer);

                // D3D11_UNORDERED_ACCESS_VIEW_DESC
                sbUAVDesc.Buffer.FirstElement = 0;
                sbUAVDesc.Buffer.Flags        = 0;
                sbUAVDesc.Buffer.NumElements  = gInstanceTransformsCount * 1;
                sbUAVDesc.Format              = DXGI_FORMAT_UNKNOWN;
                sbUAVDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
                hr |= renderSystem->m_Device->CreateUnorderedAccessView(tempBuffer, &sbUAVDesc, &instanceUAV);

                // D3D11_SHADER_RESOURCE_VIEW_DESC
                srvDesc.Buffer.ElementOffset = 0;
                srvDesc.Buffer.ElementWidth  = sizeof(XMMATRIX);
                srvDesc.Buffer.FirstElement  = 0;
                srvDesc.Buffer.NumElements   = gInstanceTransformsCount;
                srvDesc.Format               = DXGI_FORMAT_UNKNOWN;
                srvDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
                hr |= renderSystem->m_Device->CreateShaderResourceView(tempBuffer, &srvDesc, &instanceSRV);

                tempBuffer->Release();

                assert(SUCCEEDED(hr));
        }

        { // Create index steep buffer
                D3D11_BUFFER_DESC                sbDesc{};
                D3D11_SUBRESOURCE_DATA           rwData{};
                D3D11_SHADER_RESOURCE_VIEW_DESC  srvDesc{};
                D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc{};

                ID3D11Buffer* tempBuffer;
                // CD3D11_BUFFER_DESC
                sbDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
                sbDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
                sbDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
                sbDesc.StructureByteStride = sizeof(uint32_t);
                sbDesc.ByteWidth           = sizeof(uint32_t) * gInstanceTransformsCount;
                sbDesc.Usage               = D3D11_USAGE_DYNAMIC;
                // D3D11_SUBRESOURCE_DATA
                rwData.pSysMem = renderTestData.instanceIndexList.data();

                hr |= renderSystem->m_Device->CreateBuffer(&sbDesc, &rwData, &tempBuffer);

                // D3D11_UNORDERED_ACCESS_VIEW_DESC
                sbUAVDesc.Buffer.FirstElement = 0;
                sbUAVDesc.Buffer.Flags        = D3D11_BUFFER_UAV_FLAG_APPEND;
                sbUAVDesc.Buffer.NumElements  = gInstanceTransformsCount;
                sbUAVDesc.Format              = DXGI_FORMAT_UNKNOWN;
                sbUAVDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
                hr |= renderSystem->m_Device->CreateUnorderedAccessView(tempBuffer, &sbUAVDesc, &instanceIndexSteepUAV);
                hr |= renderSystem->m_Device->CreateUnorderedAccessView(tempBuffer, &sbUAVDesc, &instanceIndexFlatUAV);


                // D3D11_SHADER_RESOURCE_VIEW_DESC
                srvDesc.Buffer.ElementOffset = 0;
                srvDesc.Buffer.ElementWidth  = sizeof(uint32_t);
                srvDesc.Buffer.FirstElement  = 0;
                srvDesc.Buffer.NumElements   = gInstanceTransformsCount;
                srvDesc.Format               = DXGI_FORMAT_UNKNOWN;
                srvDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
                hr |= renderSystem->m_Device->CreateShaderResourceView(tempBuffer, &srvDesc, &instanceIndexSteepSRV);
                hr |= renderSystem->m_Device->CreateShaderResourceView(tempBuffer, &srvDesc, &instanceIndexFlatSRV);
                tempBuffer->Release();
                assert(SUCCEEDED(hr));
        }


        instanceDrawCallsData.push_back(renderTestData);
}
using namespace DirectX;

void TerrainManager::_update(float deltaTime)
{
        using namespace DirectX;

        WrapInstanceTransforms();

        ResourceManager* resourceManager = GEngine::Get()->GetResourceManager();

        XMVECTOR playerPos = GEngine::Get()
                                 ->GetSystemManager()
                                 ->GetSystem<TransformSystem>()
                                 ->GetPlayerWrapTransformHandle()
                                 .Get<TransformComponent>()
                                 ->transform.translation;

        XMVECTOR correctedTerrainPos = playerPos;
        float    cellSize            = terrainConstantBufferCPU.gWorldCellSpace * scale * 8.0f;
        XMVECTOR cellVector          = XMVectorSet(cellSize, 1.0f, cellSize, 1.0f);
        XMVECTOR modCell             = XMVectorMod(correctedTerrainPos, cellVector);
        modCell                      = XMVectorSetW(modCell, 0.0f);
        correctedTerrainPos -= modCell;
        correctedTerrainPos  = XMVectorSetY(correctedTerrainPos, 0.0f);
        TerrainMatrix.r[3]   = correctedTerrainPos;
        InverseTerrainMatrix = DirectX::XMMatrixInverse(nullptr, TerrainMatrix);

        // GEngine::Get()->m_TerrainAlpha += deltaTime * 0.04f;
        // GEngine::Get()->m_TerrainAlpha = std::min(1.0f, GEngine::Get()->m_TerrainAlpha);

        UINT stride = sizeof(TerrainVertex);
        UINT offset = 0;

        terrainConstantBufferCPU.worldView     = XMMatrixTranspose(TerrainMatrix * renderSystem->m_CachedMainViewMatrix);
        terrainConstantBufferCPU.gTerrainAlpha = GEngine::Get()->m_TerrainAlpha;

        XMStoreFloat3(&terrainConstantBufferCPU.gOriginOffset, GEngine::Get()->m_OriginOffset);


        terrainConstantBufferCPU.gScreenDimensions =
            XMFLOAT2(renderSystem->m_BackBufferWidth, renderSystem->m_BackBufferHeight);
        terrainConstantBufferCPU.gTriangleSize = 8.0f;
        renderSystem->UpdateConstantBuffer(
            terrainConstantBufferGPU, &terrainConstantBufferCPU, sizeof(terrainConstantBufferCPU));
        renderSystem->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
        renderSystem->m_Context->IASetInputLayout(inputLayout);
        renderSystem->m_Context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        renderSystem->m_Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

        renderSystem->m_Context->VSSetShader(vertexShader, nullptr, 0);
        renderSystem->m_Context->VSSetConstantBuffers(4, 1, &terrainConstantBufferGPU);
        renderSystem->m_Context->VSSetShaderResources(9, 1, &terrainSourceSRV);

        renderSystem->m_Context->HSSetShader(hullShader, nullptr, 0);
        renderSystem->m_Context->HSSetConstantBuffers(4, 1, &terrainConstantBufferGPU);
        renderSystem->m_Context->HSSetShaderResources(9, 1, &terrainSourceSRV);

        renderSystem->m_Context->DSSetConstantBuffers(4, 1, &terrainConstantBufferGPU);
        renderSystem->m_Context->DSSetShaderResources(9, 1, &terrainSourceSRV);

        renderSystem->m_Context->PSSetConstantBuffers(4, 1, &terrainConstantBufferGPU);
        renderSystem->m_Context->PSSetShaderResources(9, 1, &terrainSourceSRV);

        // Setup textures
        ID3D11ShaderResourceView* srvs[6];
        resourceManager->GetSRVs(6, mTextureHandles, srvs);
        renderSystem->m_Context->PSSetShaderResources(0, 4, srvs);
        renderSystem->m_Context->PSSetShaderResources(10, 1, &srvs[4]);

        // if (GCoreInput::GetKeyState(KeyCode::T) == KeyState::Down)


        { // Ocean
                renderSystem->m_Context->DSSetShader(oceanDomainShader, nullptr, 0);
                renderSystem->m_Context->PSSetShader(oceanPixelShader, nullptr, 0);
                renderSystem->m_Context->PSSetShaderResources(1, 1, &srvs[5]);
                // oceanMatrix.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
                XMMATRIX oceanMatrix                     = TerrainMatrix;
                renderSystem->m_ConstantBuffer_MVP.World = XMMatrixTranspose(oceanMatrix);
                renderSystem->UpdateConstantBuffer(renderSystem->m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP],
                                                   &renderSystem->m_ConstantBuffer_MVP,
                                                   sizeof(renderSystem->m_ConstantBuffer_MVP));
                renderSystem->m_Context->DrawIndexed(patchQuadCount * 4, 0, 0);
        }

        { // Terrain
                renderSystem->m_ConstantBuffer_MVP.World = XMMatrixTranspose(TerrainMatrix);
                renderSystem->UpdateConstantBuffer(renderSystem->m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP],
                                                   &renderSystem->m_ConstantBuffer_MVP,
                                                   sizeof(renderSystem->m_ConstantBuffer_MVP));
                renderSystem->m_Context->PSSetShader(pixelShader, nullptr, 0);
                renderSystem->m_Context->DSSetShader(domainShader, nullptr, 0);
                renderSystem->m_Context->DrawIndexed(patchQuadCount * 4, 0, 0);
        }
        // Draw instanced
        {
                ID3D11ShaderResourceView* nullSRV = nullptr;

                renderSystem->m_Context->VSSetShaderResources(8, 1, &nullSRV);
                renderSystem->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                // Set Compute Shaders
                ID3D11UnorderedAccessView* nullUAV = NULL;
                ComputeShader*             cs = resourceManager->GetResource<ComputeShader>(m_UpdateInstancesComputeShader);
                renderSystem->m_Context->CSSetShader(cs->m_ComputerShader, 0, 0);
                renderSystem->m_Context->CSSetConstantBuffers(
                    1, 1, &renderSystem->m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::SCENE]);
                renderSystem->m_Context->CSSetConstantBuffers(4, 1, &terrainConstantBufferGPU);

                renderSystem->m_Context->CSSetUnorderedAccessViews(0, 1, &instanceUAV, 0);
                renderSystem->m_Context->CSSetShaderResources(9, 1, &terrainSourceSRV);
                renderSystem->m_Context->CSSetShaderResources(9, 1, &terrainSourceSRV);
                // renderSystem->m_Context->CSSetShaderResources(2, 1, &instanceSRV);
                // instanceIndexSRV ?
                renderSystem->m_Context->Dispatch(gInstanceTransformsCount, 1, 1);
                renderSystem->m_Context->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);

                ID3D11HullShader*   nullHull   = nullptr;
                ID3D11DomainShader* nullDomain = nullptr;
                renderSystem->m_Context->HSSetShader(nullHull, 0, 0);
                renderSystem->m_Context->DSSetShader(nullDomain, 0, 0);

                renderSystem->m_Context->VSSetShaderResources(8, 1, &instanceSRV);
                renderSystem->m_Context->IASetInputLayout(renderSystem->m_DefaultInputLayouts[E_INPUT_LAYOUT::DEFAULT]);
                for (auto& data : instanceDrawCallsData)
                {
                        StaticMesh* sm  = resourceManager->GetResource<StaticMesh>(data.mesh);
                        Material*   mat = resourceManager->GetResource<Material>(data.material);

                        ID3D11Buffer* vertexBuffer = sm->m_VertexBuffer;
                        ID3D11Buffer* indexBuffer  = sm->m_IndexBuffer;
                        uint32_t      indexCount   = sm->m_IndexCount;
                        uint32_t      vertexSize   = sizeof(FVertex);

                        uint32_t instancecount = data.instanceCount;
                        renderSystem->m_Context->VSSetShaderResources(9, 1, &nullSRV);

                        renderSystem->m_Context->VSSetShaderResources(9, 1, &instanceIndexFlatSRV);
                        renderSystem->DrawMeshInstanced(vertexBuffer, indexBuffer, indexCount, vertexSize, mat, instancecount);
                }
        }

        // debug_renderer::AddSphere(Shapes::FSphere(test, 0.1f), 32, XMMatrixIdentity());

        renderSystem->m_Context->DSSetShader(nullptr, nullptr, 0);
        renderSystem->m_Context->HSSetShader(nullptr, nullptr, 0);
}

void TerrainManager::_shutdown()
{
        inputLayout->Release();
        vertexShader->Release();
        hullShader->Release();
        domainShader->Release();
        pixelShader->Release();
        oceanPixelShader->Release();
        oceanDomainShader->Release();

        terrainSourceSRV->Release();

        terrainIntermediateTexture->Release();
        terrainIntermediateRenderTarget->Release();
        terrainIntermediateSRV->Release();

        delete[] terrainHeightArray;

        vertexBuffer->Release();
        indexBuffer->Release();

        SAFE_RELEASE(instanceSRV);
        SAFE_RELEASE(instanceIndexSteepSRV);
        SAFE_RELEASE(instanceIndexFlatSRV);
        SAFE_RELEASE(instanceUAV);
        SAFE_RELEASE(instanceIndexSteepUAV);
        SAFE_RELEASE(instanceIndexFlatUAV);

        terrainConstantBufferGPU->Release();
        stagingTextureResource->Release();
}

void TerrainManager::GenerateInstanceTransforms(FTransform tArray[gInstanceTransformsCount])
{
        for (int i = 0; i < gInstanceTransformsCount; ++i)
        {
                float x               = 100.0f * (MathLibrary::GetRandomFloat() * 2.0f - 1.0f);
                float y               = 100.0f * (MathLibrary::GetRandomFloat() * 2.0f - 1.0f);
                tArray[i].translation = XMVectorSet(x, 0.0f, y, 1.0f);
                tArray[i].rotation = FQuaternion::RotateAxisAngle(VectorConstants::Up, MathLibrary::GetRandomFloat() * 360.0f);
                tArray[i].SetScale(MathLibrary::RandomFloatInRange(0.8f, 1.2f));
        }
}

void TerrainManager::WrapInstanceTransforms()
{
        /* playerTransform = currController->GetControlledEntity().GetComponentHandle<TransformComponent>();

         XMVECTOR& playerPos = playerTransform.Get<TransformComponent>()->transform.translation;
         float     scale     = TerrainManager::Get()->GetScale();
         XMVECTOR  min       = XMVectorSet(-0.5f * scale, 0.0f, -0.5f * scale, 0.0f);
         XMVECTOR  max       = -min;


         XMVECTOR newPlayerPos = MathLibrary::WrapPosition(playerPos, min, max);*/
}

void TerrainManager::CreateVertexBuffer(ID3D11Buffer** buffer, unsigned int squareDimensions, float waterLevel, float scale)
{
        TerrainVertex* patchVerts = new TerrainVertex[squareDimensions * squareDimensions];
        float          midX       = scale / 2;
        float          midZ       = scale / 2;

        float patchX = scale / (squareDimensions - 1);
        float patchZ = scale / (squareDimensions - 1);
        float du     = 1.0f / (squareDimensions - 1);
        float dv     = 1.0f / (squareDimensions - 1);

        for (unsigned int i = 0; i < squareDimensions; ++i)
        {
                float z = midZ - i * patchZ;
                for (unsigned int j = 0; j < squareDimensions; ++j)
                {
                        float x      = -midX + j * patchX;
                        int   vertId = i * squareDimensions + j;

                        TerrainVertex vert;
                        vert.pos = {x, waterLevel, z};
                        vert.tex = {j * du, i * dv};

                        patchVerts[vertId] = vert;
                }
        }

        D3D11_BUFFER_DESC bd{};
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(TerrainVertex) * squareDimensions * squareDimensions;
        bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData{};
        InitData.pSysMem = patchVerts;

        renderSystem->GetDevice()->CreateBuffer(&bd, &InitData, &vertexBuffer);

        delete[] patchVerts;
}

void TerrainManager::CreateIndexBuffer(ID3D11Buffer** buffer, unsigned int squareDimensions)
{
        uint32_t* indices = new uint32_t[squareDimensions * squareDimensions * 4];
        int       n       = 0;
        for (unsigned int i = 0; i < squareDimensions - 1; ++i)
        {
                for (unsigned int j = 0; j < squareDimensions - 1; ++j)
                {
                        indices[n]     = i * squareDimensions + j;
                        indices[n + 1] = i * squareDimensions + j + 1;
                        indices[n + 2] = (i + 1) * squareDimensions + j + 1;
                        indices[n + 3] = (i + 1) * squareDimensions + j;
                        n += 4;
                }
        }
        D3D11_BUFFER_DESC bd{};
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(uint32_t) * squareDimensions * squareDimensions * 4;
        bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData{};
        InitData.pSysMem = indices;

        renderSystem->GetDevice()->CreateBuffer(&bd, &InitData, &indexBuffer);

        delete[] indices;
}

TerrainManager* TerrainManager::Get()
{
        return instance;
}


float BilinearFilter(float x, float y, float* array, unsigned int width, unsigned int height)
{
        int       px     = (int)x; // floor of x
        int       py     = (int)y; // floor of y
        const int stride = width;

        int nX = px % width;
        int pX = (px + 1) % width;
        int nY = py % height;
        int pY = (py + 1) % height;
        // load the four neighboring pixels
        const float& p1 = array[nX + nY * width];
        const float& p2 = array[pX + nY * width];
        const float& p3 = array[nX + pY * width];
        const float& p4 = array[pX + pY * width];

        // Calculate the weights for each pixel
        float fx  = x - px;
        float fy  = y - py;
        float fx1 = 1.0f - fx;
        float fy1 = 1.0f - fy;

        float w1 = fx1 * fy1;
        float w2 = fx * fy1;
        float w3 = fx1 * fy;
        float w4 = fx * fy;

        // Calculate the weighted sum of pixels (for each color channel)
        float outheight = p1 * w1 + p2 * w2 + p3 * w3 + p4 * w4;

        return outheight;
}

DirectX::XMVECTOR TerrainManager::AlignPositionToTerrain(const DirectX::XMVECTOR& pos)
{
        using namespace DirectX;
        DirectX::XMVECTOR correctPos = DirectX::XMVector3TransformNormal(pos, InverseTerrainMatrix);
        XMFLOAT3          fPos;

        float bounds = 0.5f * 8000.0f;

        XMStoreFloat3(&fPos, correctPos + XMVectorSet(bounds, 0.0f, bounds, 0.0f));

        float div   = terrainConstantBufferCPU.gWorldCellSpace * textureDimensions / intermediateMipDimensions;
        float currX = fPos.x / div;
        float currY = fPos.z / div;


        float height = BilinearFilter(currX, currY, terrainHeightArray, intermediateMipDimensions, intermediateMipDimensions);

        height = terrainConstantBufferCPU.gTerrainAlpha * 2625.f * height + WaterLevel * terrainConstantBufferCPU.gTerrainAlpha;

        return DirectX::XMVectorSetY(pos, std::max(height * scale, 0.0f) + groundOffset);
}

void TerrainManager::Initialize(RenderSystem* rs)
{
        instance = new TerrainManager;
        instance->_initialize(rs);
}

void TerrainManager::Update(float deltaTime)
{
        GEngine::Get()->m_MainThreadProfilingContext.Begin("TerrainManager", "TerrainManager");
        instance->_update(deltaTime);
        GEngine::Get()->m_MainThreadProfilingContext.End();
}

void TerrainManager::Shutdown()
{
        instance->_shutdown();
        delete instance;
}
