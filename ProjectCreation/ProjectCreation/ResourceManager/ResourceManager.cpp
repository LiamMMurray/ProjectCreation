#include "ResourceManager.h"
#include <assert.h>

#include "../Engine/GEngine.h"
#include "../Rendering/RenderingSystem.h"

#include "../FileIO/FileIO.h"

#include <d3d11_1.h>

#include "Material.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Texture2D.h"
#include "PixelShader.h"
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

        return outputHandle;
}

ResourceHandle ResourceManager::LoadTexture2D(const char* name)
{
        return ResourceHandle();
}

ResourceHandle ResourceManager::LoadVertexShader(const char* name)
{
        return ResourceHandle();
}

ResourceHandle ResourceManager::LoadPixelShader(const char* name)
{
        return ResourceHandle();
}

ResourceHandle ResourceManager::LoadStaticMesh(const char* name)
{
        FileIO::FStaticMeshData meshData;
        EResult                 result = FileIO::LoadStaticMeshDataFromFile(name, &meshData);

        assert(result.m_Flags == ERESULT_FLAG::SUCCESS);

        CRenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<CRenderSystem>();

        D3D11_BUFFER_DESC bd{};
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = UINT(sizeof(FVertex) * meshData.vertices.size());
        bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA InitData{};
        InitData.pSysMem = meshData.vertices.data();

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

        StaticMesh* resource = container->GetResource(outputHandle);

        // renderSystem->m_Device->CreateBuffer(&bd, &InitData, resource.);

        return outputHandle;
}

void ResourceManager::Initialize()
{}

void ResourceManager::Shutdown()
{}
