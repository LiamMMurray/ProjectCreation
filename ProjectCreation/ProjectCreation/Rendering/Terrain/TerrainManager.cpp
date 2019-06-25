#include "TerrainManager.h"
#include <d3d11.h>
#include <d3d11_1.h>
#include "../..//FileIO/FileIO.h"
#include "../..//Utility/StringUtility.h"
#include "..//..//Engine/CoreInput/CoreInput.h"
#include "..//..//Engine/GEngine.h"
#include "..//RenderingSystem.h"

TerrainManager* TerrainManager::instance;

void TerrainManager::_initialize(RenderSystem* rs)
{
        renderSystem = rs;

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
        rs->GetDevice()->CreateHullShader(hullShaderData.bytes.data(), hullShaderData.bytes.size(), nullptr, &hullShader);

        FileIO::FShaderData pixelShaderData;
        FileIO::LoadShaderDataFromFile("Terrain", "_PS", &pixelShaderData);
        rs->GetDevice()->CreatePixelShader(pixelShaderData.bytes.data(), pixelShaderData.bytes.size(), nullptr, &pixelShader);

        D3D11_INPUT_ELEMENT_DESC vLayout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        rs->GetDevice()->CreateInputLayout(
            vLayout, 2, vertexShaderData.bytes.data(), vertexShaderData.bytes.size(), &inputLayout);

        std::wstring str = L"../Assets/Textures/TerrainTest.dds";

        ID3D11Resource* sourceTexture;
        DirectX::CreateDDSTextureFromFile(renderSystem->GetDevice(), str.c_str(), &sourceTexture, &terrainSourceSRV);
        terrainSourceTexture = (ID3D11Texture2D*)sourceTexture;

        D3D11_TEXTURE2D_DESC desc;
        terrainSourceTexture->GetDesc(&desc);
        UINT texwidth  = desc.Width;
        UINT texheight = desc.Height;

        patchSquareDimensions = ((texwidth - 1) / patchCells) + 1;
        patchQuadCount        = (patchSquareDimensions - 1) * (patchSquareDimensions - 1);


        assert(texwidth == texheight);

        CreateVertexBuffer(&vertexBuffer, patchSquareDimensions, 1115.0f, 8000.0f);
        CreateIndexBuffer(&indexBuffer, patchSquareDimensions);

        D3D11_BUFFER_DESC bd{};
        bd.Usage          = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth      = sizeof(CTerrainInfoBuffer);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        renderSystem->GetDevice()->CreateBuffer(&bd, nullptr, &terrainConstantBufferGPU);

        terrainConstantBufferCPU.gTexelCellSpaceU = 1.0f / texwidth;
        terrainConstantBufferCPU.gTexelCellSpaceV = 1.0f / texheight;
        terrainConstantBufferCPU.gTexScale        = 64.0f;
        terrainConstantBufferCPU.gWorldCellSpace  = 8000.0f * scale / patchSquareDimensions;
}


void TerrainManager::_update(float deltaTime)
{
        using namespace DirectX;

        UINT stride = sizeof(TerrainVertex);
        UINT offset = 0;

        renderSystem->m_ConstantBuffer_MVP.World = XMMatrixTranspose(XMMatrixScaling(scale, scale, scale));
        renderSystem->UpdateConstantBuffer(renderSystem->m_BasePassConstantBuffers[E_CONSTANT_BUFFER_BASE_PASS::MVP],
                                           &renderSystem->m_ConstantBuffer_MVP,
                                           sizeof(renderSystem->m_ConstantBuffer_MVP));

        terrainConstantBufferCPU._TesselationFactor = renderSystem->m_BackBufferHeight / 16.f;
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

        renderSystem->m_Context->DSSetShader(domainShader, nullptr, 0);
        renderSystem->m_Context->DSSetConstantBuffers(4, 1, &terrainConstantBufferGPU);
        renderSystem->m_Context->DSSetShaderResources(9, 1, &terrainSourceSRV);

        renderSystem->m_Context->PSSetShader(pixelShader, nullptr, 0);
        renderSystem->m_Context->PSSetConstantBuffers(4, 1, &terrainConstantBufferGPU);
        renderSystem->m_Context->PSSetShaderResources(9, 1, &terrainSourceSRV);

        if (GCoreInput::GetKeyState(KeyCode::T) == KeyState::Down)
                renderSystem->m_Context->DrawIndexed(patchQuadCount * 4, 0, 0);

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

        terrainSourceTexture->Release();
        terrainSourceSRV->Release();

        // terrainTargetTexture->Release();
        // terrainTargetRenderTarget->Release();

        vertexBuffer->Release();
        indexBuffer->Release();

        terrainConstantBufferGPU->Release();
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
                        vert.pos = {x, 0, z};
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

void TerrainManager::Initialize(RenderSystem* rs)
{
        instance = new TerrainManager;
        instance->_initialize(rs);
}

void TerrainManager::Update(float deltaTime)
{
        instance->_update(deltaTime);
}

void TerrainManager::Shutdown()
{
        instance->_shutdown();
        delete instance;
}
