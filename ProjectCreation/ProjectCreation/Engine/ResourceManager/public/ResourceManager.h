#pragma once

#include "Resource.h"

#include <assert.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <MemoryLeakDetection.h>

namespace Animation
{
        struct FSkeleton;
} // namespace Animation

struct ID3D11ShaderResourceView;

class ResourceContainerBase
{
    public:
        virtual ~ResourceContainerBase(){};
};

struct Material;

template <class T>
class ResourceContainer : public ResourceContainerBase
{
        static_assert(std::is_base_of<Resource<T>, T>::value, "Resource container can only store derived Resource types");


        std::vector<T>                                  m_Container;
        std::unordered_set<ResourceHandle>              m_HandleSet;
        std::unordered_map<std::string, ResourceHandle> m_NameTable;

    public:
        virtual ~ResourceContainer() override
        {
                for (auto& resource : m_Container)
                        resource.Release();
        };

        T* GetResource(const ResourceHandle& handle)
        {
                auto it = m_HandleSet.find(handle);
                assert(it != m_HandleSet.end() && "Resource doesn't exist");
                return &m_Container[handle.m_Data];
        }

        void DestroyResource(const ResourceHandle& handle)
        {
                auto it = m_HandleSet.find(handle);
                assert(it != m_HandleSet.end() && "Resource doesn't exist");
                Resource* resource  = GetResource(handle);
                size_t    lastIndex = m_Container.size() - 1;
                if (it->second != lastIndex)
                {
                        m_Container[it->second] = m_Container[lastIndex];
                }


                m_Container.erase(m_Container.end() - 1);
                m_HandleSet.erase(handle);
                m_NameTable.erase(resource->GetName());

                return resource;
        }

        T* AcquireResource(const ResourceHandle& handle)
        {
                T* resource = GetResource(handle);
                resource->AcquireHandle();
                return resource;
        }

        uint16_t ReleaseResource(const ResourceHandle& handle)
        {
                T*      resource = GetResource(handle);
                int16_t refCount = resource->ReleaseHandle();

                if (refCount <= 0)
                        DestroyResource(handle);

                return refCount;
        }

        ResourceHandle CreateResource(std::string name)
        {
                assert(m_NameTable.find(name) == m_NameTable.end());

                ResourceHandle outHandle;
                auto           dataOld = m_Container.data();
                outHandle.m_Data       = (uint32_t)m_Container.size();
                m_Container.emplace_back(T());
                size_t index    = m_Container.size() - 1;
                T&     resource = m_Container.back();
                resource.Init(name, outHandle);
                auto dataNew      = m_Container.data();
                m_NameTable[name] = outHandle;
                m_HandleSet.insert(outHandle);
                return outHandle;
        }

        friend class ResourceManager;
};

class RenderSystem;

class ResourceManager
{
        std::unordered_map<ResourceTypeID, ResourceContainerBase*> m_Containers;

        template <typename T>
        ResourceHandle CreateResource();

        template <typename T>
        ResourceContainer<T>* GetResourceContainer();

    public:
        ResourceHandle LoadMaterial(const char* name);
        ResourceHandle LoadTexture2D(const char* name);
        ResourceHandle LoadVertexShader(const char* name);
        ResourceHandle LoadPixelShader(const char* name);
        ResourceHandle LoadComputeShader(const char* name);
        ResourceHandle LoadGeometryShader(const char* name);
        ResourceHandle LoadStaticMesh(const char* name);
        ResourceHandle LoadSkeletalMesh(const char* name);
        ResourceHandle LoadAnimationClip(const char* name, const Animation::FSkeleton* skeleton);

        template <typename T>
        ResourceHandle CopyResource(ResourceHandle, const char* name);

        void GetSRVsFromMaterial(Material* material, ID3D11ShaderResourceView** srvsOut);
        void GetSRVs(unsigned int count, ResourceHandle* textureHandles, ID3D11ShaderResourceView** srvsOut);

        template <typename T>
        T* AcquireResource(const ResourceHandle& handle);

        template <typename T>
        uint16_t ReleaseResource(const ResourceHandle& handle);

        template <typename T>
        T* GetResource(const ResourceHandle& handle);


        void Initialize();
        void Shutdown();
};
template <typename T>
ResourceHandle ResourceManager::CreateResource()
{
        ResourceContainer<T>* container = GetResourceContainer<T>();
        return container->CreateResource();
}

template <typename T>
ResourceContainer<T>* ResourceManager::GetResourceContainer()
{
        ResourceTypeID ID = T::GetTypeID();

        auto                  it = this->m_Containers.find(ID);
        ResourceContainer<T>* rc = nullptr;

        if (it == this->m_Containers.end())
        {
                rc                     = new ResourceContainer<T>();
                this->m_Containers[ID] = rc;
        }
        else
                rc = (ResourceContainer<T>*)it->second;

        assert(rc != nullptr && "Failed to create ComponentContainer<T>");
        return rc;
}

template <typename T>
inline ResourceHandle ResourceManager::CopyResource(ResourceHandle source, const char* name)
{
        ResourceContainer<T>* container      = GetResourceContainer<T>();
        ResourceHandle        targetHandle   = container->CreateResource(name);
        T*                    sourceResource = container->GetResource(source);
        T*                    targetResource = container->GetResource(targetHandle);
        targetResource->Copy(sourceResource);


        return targetHandle;
}

template <typename T>
T* ResourceManager::AcquireResource(const ResourceHandle& handle)
{
        ResourceContainer<T>* container = GetResourceContainer<T>();
        return container->AcquireResource(handle);
}

template <typename T>
uint16_t ResourceManager::ReleaseResource(const ResourceHandle& handle)
{
        ResourceContainer<T>* container = GetResourceContainer<T>();
        return container->ReleaseResource(handle);
}

template <typename T>
T* ResourceManager::GetResource(const ResourceHandle& handle)
{
        ResourceContainer<T>* container = GetResourceContainer<T>();
        return container->GetResource(handle);
}
