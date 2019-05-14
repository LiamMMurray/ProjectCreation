#pragma once

#include "Resource.h"

#include <assert.h>
#include <string>
#include <unordered_map>
#include <vector>

class ResourceContainerBase
{
    public:
        virtual ~ResourceContainerBase(){};
};

template <class T>
class ResourceContainer : public ResourceContainerBase
{
        static_assert(std::is_base_of<Resource<T>, T>::value, "Resource container can only store derived Resource types");


        std::vector<T>                                  m_Container;
        std::unordered_map<ResourceHandle, T*>          m_ResourceTable;
        std::unordered_map<std::string, ResourceHandle> m_NameTable;

        void UpdateHandles()
        {
                for (auto& resource : m_Container)
                {
                        m_ResourceTable[resource.GetHandle()] = &resource;
                }
        }

    public:
        virtual ~ResourceContainer() override
        {
                for (auto& resource : m_Container)
                        resource.Release();
        };

        T* GetResource(ResourceHandle handle)
        {
                T* resource = m_ResourceTable[handle];
                assert(resource && "Resource doesn't exist");
                return m_ResourceTable[handle];
        }

        T* DestroyResource(ResourceHandle handle)
        {
                T* resource = m_ResourceTable[handle];
                assert(resource && "Resource doesn't exist");
                m_ResourceTable.erase(handle);
                m_NameTable.erase(resource->GetName());
                UpdateHandles();

				return resource;
        }

        T* AcquireResource(ResourceHandle handle)
        {
                T* resource = m_ResourceTable[handle];
                assert(resource && "Resource doesn't exist");
                resource->AcquireHandle();
                return resource;
        }

        uint16_t ReleaseResource(ResourceHandle handle)
        {
                T* resource = m_ResourceTable[handle];
                assert(resource && "Resource doesn't exist");
                int16_t refCount = resource->ReleaseHandle();

                if (refCount <= 0)
                        DestroyResource(handle);

                return refCount;
        }

        ResourceHandle CreateResource(std::string name)
        {
                ResourceHandle outHandle;
                auto           dataOld = m_Container.data();
                outHandle.m_Id         = (uint32_t)m_Container.size();
                m_Container.emplace_back(T());
                T& resource = m_Container.back();
                resource.Init(name, outHandle);
                auto dataNew = m_Container.data();

                m_NameTable[name]          = outHandle;
                m_ResourceTable[outHandle] = &resource;

                if (dataOld != dataNew)
                {
                        UpdateHandles();
                }

                return outHandle;
        }

        friend class ResourceManager;
};

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
        ResourceHandle LoadStaticMesh(const char* name);

        template <typename T>
        T* AcquireResource(ResourceHandle handle);

        template <typename T>
        uint16_t ReleaseResource(ResourceHandle handle);

        template <typename T>
        T* GetResource(ResourceHandle handle);


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
T* ResourceManager::AcquireResource(ResourceHandle handle)
{
        ResourceContainer<T>* container = GetResourceContainer<T>();
        return container->AcquireResource(handle);
}

template <typename T>
uint16_t ResourceManager::ReleaseResource(ResourceHandle handle)
{
        ResourceContainer<T>* container = GetResourceContainer<T>();
        return container->ReleaseResource(handle);
}

template <typename T>
T* ResourceManager::GetResource(ResourceHandle handle)
{
        ResourceContainer<T>* container = GetResourceContainer<T>();
        return container->GetResource(handle);
}
