#include "ResourceManager.h"
#include <assert.h>

#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"

#include "../Utility/StringUtility.h"

#include "../FileIO/FileIO.h"

#include <d3d11_1.h>

#include "Material.h"
#include "PixelShader.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Texture2D.h"
#include "VertexShader.h"


ResourceHandle ResourceManager::LoadMaterial(const char* name)
{
        FileIO::FMaterialData materialData;
        EResult               result = FileIO::LoadMaterialDataFromFile(name, &materialData);

        assert(result.m_Flags == ERESULT_FLAG::SUCCESS);

        CRenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<CRenderSystem>();

        auto container = GetResourceContainer<Material>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle = container->CreateResource(name);
        }
        else
        {
                outputHandle = it->second;
        }

        Material* resource = container->GetResource(outputHandle);

        resource->m_VertexShaderHandle = LoadVertexShader(name);

        return outputHandle;
}

ResourceHandle ResourceManager::LoadTexture2D(const char* name)
{
        FileIO::FMaterialData materialData;

        CRenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<CRenderSystem>();

        auto container = GetResourceContainer<Texture2D>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle = container->CreateResource(name);
        }
        else
        {
                outputHandle = it->second;
        }

        Texture2D* resource = container->GetResource(outputHandle);

        ID3D11Resource* texture;
        std::wstring    str = L"../Textures/" + StringUtility::utf8_decode(name) + L".dds";
        DirectX::CreateDDSTextureFromFile(renderSystem->m_Device, str.c_str(), &texture, &resource->m_SRV);
        texture->Release();

        return outputHandle;
}

ResourceHandle ResourceManager::LoadVertexShader(const char* name)
{
        FileIO::FMaterialData materialData;

        CRenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<CRenderSystem>();

        auto container = GetResourceContainer<VertexShader>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle = container->CreateResource(name);
        }
        else
        {
                outputHandle = it->second;
        }

        VertexShader* resource = container->GetResource(outputHandle);

        FileIO::FShaderData shaderData;
        FileIO::LoadShaderDataFromFile(name, "_VS", &shaderData);

        renderSystem->m_Device->CreateVertexShader(
            shaderData.bytes.data(), shaderData.bytes.size(), nullptr, &resource->m_VertexShader);

        return outputHandle;
}

ResourceHandle ResourceManager::LoadPixelShader(const char* name)
{
        FileIO::FMaterialData materialData;

        CRenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<CRenderSystem>();

        auto container = GetResourceContainer<PixelShader>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle = container->CreateResource(name);
        }
        else
        {
                outputHandle = it->second;
        }

        PixelShader* resource = container->GetResource(outputHandle);

        FileIO::FShaderData shaderData;
        FileIO::LoadShaderDataFromFile(name, "_PS", &shaderData);

        renderSystem->m_Device->CreatePixelShader(
            shaderData.bytes.data(), shaderData.bytes.size(), nullptr, &resource->m_PixelShader);

        return outputHandle;
}

ResourceHandle ResourceManager::LoadStaticMesh(const char* name)
{
        FileIO::FStaticMeshData meshData;
        EResult                 result = FileIO::LoadStaticMeshDataFromFile(name, &meshData);

        assert(result.m_Flags == ERESULT_FLAG::SUCCESS);

        CRenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<CRenderSystem>();


        auto container = GetResourceContainer<StaticMesh>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle = container->CreateResource(name);
        }
        else
        {
                outputHandle = it->second;
        }

        StaticMesh* resource    = container->GetResource(outputHandle);
        resource->m_VertexCount = (uint32_t)meshData.vertices.size();
        resource->m_IndexCount  = (uint32_t)meshData.indices.size();

        D3D11_BUFFER_DESC bd{};
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = UINT(sizeof(FVertex) * resource->m_VertexCount);
        bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData{};
        InitData.pSysMem = meshData.vertices.data();

        renderSystem->m_Device->CreateBuffer(&bd, &InitData, &resource->m_VertexBuffer);

        bd.ByteWidth     = UINT(sizeof(uint32_t) * resource->m_IndexCount);
        bd.BindFlags     = D3D11_BIND_INDEX_BUFFER;
        InitData.pSysMem = meshData.indices.data();

        renderSystem->m_Device->CreateBuffer(&bd, &InitData, &resource->m_IndexBuffer);

        return outputHandle;
}

ResourceHandle ResourceManager::LoadAnimationClip(const char* name)
{
        FileIO::FMaterialData materialData;

        CRenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<CRenderSystem>();

        auto container = GetResourceContainer<PixelShader>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle = container->CreateResource(name);
        }
        else
        {
                outputHandle = it->second;
        }

        PixelShader* resource = container->GetResource(outputHandle);

        FileIO::FShaderData shaderData;
        FileIO::LoadShaderDataFromFile(name, "_PS", &shaderData);

        renderSystem->m_Device->CreatePixelShader(
            shaderData.bytes.data(), shaderData.bytes.size(), nullptr, &resource->m_PixelShader);

        return outputHandle;
}

void ResourceManager::Initialize()
{}

void ResourceManager::Shutdown()
{
        for (auto& it : m_Containers)
        {
                delete it.second;
        }
}
