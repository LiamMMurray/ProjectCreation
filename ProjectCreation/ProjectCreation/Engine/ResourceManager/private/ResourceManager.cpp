#include <ResourceManager.h>
#include <assert.h>

#include <RenderingSystem.h>
#include <GEngine.h>

#include <StringUtility.h>

#include <FileIO.h>

#include <d3d11_1.h>

#include <AnimationClip.h>
#include <ComputeShader.h>
#include <GeometryShader.h>
#include <Material.h>
#include <PixelShader.h>
#include <SkeletalMesh.h>
#include <StaticMesh.h>
#include <Texture2D.h>
#include <VertexShader.h>


ResourceHandle ResourceManager::LoadMaterial(const char* name)
{
        auto container = GetResourceContainer<Material>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle = container->CreateResource(name);

                FileIO::FMaterialData materialData;
                EResult               result = FileIO::LoadMaterialDataFromFile(name, &materialData);

                assert(result.m_Flags == ERESULT_FLAG::SUCCESS);
                Material* resource             = container->GetResource(outputHandle);
                resource->m_VertexShaderHandle = LoadVertexShader(materialData.vertexShader.data());
                resource->m_PixelShaderHandle  = LoadPixelShader(materialData.pixelShader.data());
                resource->m_SurfaceProperties  = materialData.surfaceProperties;
                // resource->m_SurfaceProperties.emissiveColor = DirectX::XMFLOAT3(20.0f, 20.0f, 20.0f);

                resource->m_TextureUsed = 0;
                for (auto desc : materialData.textureDescs)
                {
                        resource->m_TextureHandles[int(desc.textureType)] = LoadTexture2D(desc.filePath.data());
                        resource->m_TextureUsed |= 1 << int(desc.textureType);
                }
        }
        else
        {
                outputHandle = it->second;
        }


        return outputHandle;
}

ResourceHandle ResourceManager::LoadTexture2D(const char* name)
{
        auto container = GetResourceContainer<Texture2D>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                RenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                outputHandle = container->CreateResource(name);

                FileIO::FMaterialData materialData;
                Texture2D*            resource = container->GetResource(outputHandle);

                ID3D11Resource* texture;
                std::wstring    str = L"../Assets/Textures/" + StringUtility::utf8_decode(name) + L".dds";
                HRESULT hr = DirectX::CreateDDSTextureFromFile(renderSystem->m_Device, str.c_str(), &texture, &resource->m_SRV);
                assert(SUCCEEDED(hr));
                texture->Release();
        }
        else
        {
                outputHandle = it->second;
        }


        return outputHandle;
}

ResourceHandle ResourceManager::LoadVertexShader(const char* name)
{


        auto container = GetResourceContainer<VertexShader>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle               = container->CreateResource(name);
                RenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                FileIO::FMaterialData materialData;
                VertexShader*         resource = container->GetResource(outputHandle);

                FileIO::FShaderData shaderData;
                FileIO::LoadShaderDataFromFile(name, "_VS", &shaderData);

                HRESULT hr = renderSystem->m_Device->CreateVertexShader(
                    shaderData.bytes.data(), shaderData.bytes.size(), nullptr, &resource->m_VertexShader);

                assert(SUCCEEDED(hr));
        }
        else
        {
                outputHandle = it->second;
        }


        return outputHandle;
}

ResourceHandle ResourceManager::LoadPixelShader(const char* name)
{
        auto container = GetResourceContainer<PixelShader>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle               = container->CreateResource(name);
                RenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                FileIO::FMaterialData materialData;
                PixelShader*          resource = container->GetResource(outputHandle);

                FileIO::FShaderData shaderData;
                FileIO::LoadShaderDataFromFile(name, "_PS", &shaderData);

                renderSystem->m_Device->CreatePixelShader(
                    shaderData.bytes.data(), shaderData.bytes.size(), nullptr, &resource->m_PixelShader);
        }
        else
        {
                outputHandle = it->second;
        }


        return outputHandle;
}

ResourceHandle ResourceManager::LoadComputeShader(const char* name)
{
        auto container = GetResourceContainer<ComputeShader>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {

                if (it == container->m_NameTable.end())
                {
                        outputHandle               = container->CreateResource(name);
                        RenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                        FileIO::FMaterialData materialData;
                        ComputeShader*        resource = container->GetResource(outputHandle);

                        FileIO::FShaderData shaderData;
                        FileIO::LoadShaderDataFromFile(name, "_CS", &shaderData);

                        renderSystem->m_Device->CreateComputeShader(
                            shaderData.bytes.data(), shaderData.bytes.size(), nullptr, &resource->m_ComputerShader);
                }
                else
                {
                        outputHandle = it->second;
                }
        }
        return outputHandle;
}

ResourceHandle ResourceManager::LoadGeometryShader(const char* name)
{
        auto container = GetResourceContainer<GeometryShader>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {

                if (it == container->m_NameTable.end())
                {
                        outputHandle               = container->CreateResource(name);
                        RenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                        FileIO::FMaterialData materialData;
                        GeometryShader*       resource = container->GetResource(outputHandle);

                        FileIO::FShaderData shaderData;
                        FileIO::LoadShaderDataFromFile(name, "_GS", &shaderData);

                        renderSystem->m_Device->CreateGeometryShader(
                            shaderData.bytes.data(), shaderData.bytes.size(), nullptr, &resource->m_GeometryShader);
                }
                else
                {
                        outputHandle = it->second;
                }
        }
        return outputHandle;
}

ResourceHandle ResourceManager::LoadStaticMesh(const char* name)
{


        auto container = GetResourceContainer<StaticMesh>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                HRESULT hr;

                outputHandle               = container->CreateResource(name);
                RenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                FileIO::FStaticMeshData meshData;
                EResult                 result = FileIO::LoadStaticMeshDataFromFile(name, &meshData);

                assert(result.m_Flags == ERESULT_FLAG::SUCCESS);
                StaticMesh* resource = container->GetResource(outputHandle);

                resource->m_VertexCount = (uint32_t)meshData.vertices.size();
                resource->m_IndexCount  = (uint32_t)meshData.indices.size();

                D3D11_BUFFER_DESC bd{};
                bd.Usage          = D3D11_USAGE_DEFAULT;
                bd.ByteWidth      = UINT(sizeof(FVertex) * resource->m_VertexCount);
                bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
                bd.CPUAccessFlags = 0;

                D3D11_SUBRESOURCE_DATA InitData{};
                InitData.pSysMem = meshData.vertices.data();

                hr = renderSystem->m_Device->CreateBuffer(&bd, &InitData, &resource->m_VertexBuffer);
                assert(SUCCEEDED(hr));

                bd.ByteWidth     = UINT(sizeof(uint32_t) * resource->m_IndexCount);
                bd.BindFlags     = D3D11_BIND_INDEX_BUFFER;
                InitData.pSysMem = meshData.indices.data();

                hr = renderSystem->m_Device->CreateBuffer(&bd, &InitData, &resource->m_IndexBuffer);
                assert(SUCCEEDED(hr));
        }
        else
        {
                outputHandle = it->second;
        }


        return outputHandle;
}

ResourceHandle ResourceManager::LoadSkeletalMesh(const char* name)
{
        auto container = GetResourceContainer<SkeletalMesh>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                HRESULT hr;

                outputHandle               = container->CreateResource(name);
                RenderSystem* renderSystem = GEngine::Get()->GetSystemManager()->GetSystem<RenderSystem>();

                FileIO::FSkeletalMeshData meshData;
                EResult                   result = FileIO::LoadSkeletalMeshDataFromFile(name, &meshData);
                assert(result.m_Flags == ERESULT_FLAG::SUCCESS);

                SkeletalMesh* resource                       = container->GetResource(outputHandle);
                resource->m_BindPoseSkeleton.inverseBindPose = meshData.inverseJoints;
                resource->m_BindPoseSkeleton.jointTransforms = meshData.joints;
                resource->m_VertexCount                      = (uint32_t)meshData.vertices.size();
                resource->m_IndexCount                       = (uint32_t)meshData.indices.size();

                D3D11_BUFFER_DESC bd{};
                bd.Usage          = D3D11_USAGE_DEFAULT;
                bd.ByteWidth      = UINT(sizeof(FSkinnedVertex) * resource->m_VertexCount);
                bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
                bd.CPUAccessFlags = 0;

                D3D11_SUBRESOURCE_DATA InitData{};
                InitData.pSysMem = meshData.vertices.data();

                hr = renderSystem->m_Device->CreateBuffer(&bd, &InitData, &resource->m_VertexBuffer);

                assert(SUCCEEDED(hr));

                bd.ByteWidth     = UINT(sizeof(uint32_t) * resource->m_IndexCount);
                bd.BindFlags     = D3D11_BIND_INDEX_BUFFER;
                InitData.pSysMem = meshData.indices.data();

                hr = renderSystem->m_Device->CreateBuffer(&bd, &InitData, &resource->m_IndexBuffer);
                assert(SUCCEEDED(hr));
        }
        else
        {
                outputHandle = it->second;
        }


        return outputHandle;
}

ResourceHandle ResourceManager::LoadAnimationClip(const char* name, const Animation::FSkeleton* skeleton)
{
        auto container = GetResourceContainer<AnimationClip>();
        auto it        = container->m_NameTable.find(name);

        ResourceHandle outputHandle;

        if (it == container->m_NameTable.end())
        {
                outputHandle = container->CreateResource(name);

                Animation::FAnimClip animClip;
                FileIO::ImportAnimClipData(name, animClip, *skeleton);
                AnimationClip* resource = container->GetResource(outputHandle);
                resource->m_AnimClip    = animClip;
        }
        else
        {
                outputHandle = it->second;
        }


        return outputHandle;
}

void ResourceManager::GetSRVsFromMaterial(Material* material, ID3D11ShaderResourceView** srvsOut)
{
        ResourceContainer<Texture2D>* container = GetResourceContainer<Texture2D>();


        for (unsigned int i = 0; i < (unsigned int)ETexture2DType::COUNT; ++i)
        {
                if (material->m_TextureUsed & (1 << i))
                {
                        srvsOut[i] = container->GetResource(material->m_TextureHandles[i])->m_SRV;
                }
                else
                {
                        srvsOut[i] = nullptr;
                }
        }
}

void ResourceManager::GetSRVs(unsigned int count, ResourceHandle* textureHandles, ID3D11ShaderResourceView** srvsOut)
{
        ResourceContainer<Texture2D>* container = GetResourceContainer<Texture2D>();
        for (unsigned int i = 0; i < count; ++i)
        {
                srvsOut[i] = container->GetResource(textureHandles[i])->m_SRV;
        }
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
