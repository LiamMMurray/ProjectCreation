#pragma once
#include <assert.h>
#include <algorithm>
#include <SpookyHashV2.h>
#include <Range.h>
#include <TypeIndexFactory.h>
#include "ComponentHandle.h"
#include "IPoolElement.h"
#include <ECSPools.h>

#include <limits>
namespace std
{
        template <>
        struct hash<ComponentHandle>
        {
                size_t operator()(const ComponentHandle& h) const
                {
                        return SpookyHash::Hash64(&h, sizeof(h), 0);
                }
        };
        template <>
        struct hash<EntityHandle>
        {
                size_t operator()(const EntityHandle& h) const
                {
                        return SpookyHash::Hash64(&h, sizeof(h), 0);
                }
        };
} // namespace std

struct HandleManager
{
        NMemory::index                             m_PoolCount;
        NMemory::NPools::RandomAccessPools&        m_ComponentRandomAccessPools;
        NMemory::NPools::RandomAccessPools&        m_EntityRandomAccessPools;
        NMemory::MemoryStack&                      m_MemoryStack;
        NMemory::NPools::pool_descs                m_PoolDescs;
        std::vector<std::pair<unsigned, unsigned>> d_debug_deleted_components;

        HandleManager(NMemory::NPools::RandomAccessPools& componentRandomAccessPools,
                      NMemory::NPools::RandomAccessPools& entityRandomAccessPools,
                      NMemory::MemoryStack&               pool_memory);

        ~HandleManager();

        template <typename T>
        T* GetComponent(ComponentHandle handle);

        Entity* GetEntity(EntityHandle handle);

        template <typename T>
        ComponentHandle AddComponent(EntityHandle parentHandle);

        EntityHandle CreateEntity(EntityHandle parentHandle = -1);

        void FreeComponent(ComponentHandle handle);

        void ReleaseComponentHandle(ComponentHandle handle);

        void ReleaseEntityHandle(EntityHandle handle);

        bool IsActive(ComponentHandle handle);

        void SetIsActive(ComponentHandle handle, bool isActive);

        void FreeEntity(EntityHandle handle);

        bool IsActive(EntityHandle handle);

        void SetIsActive(EntityHandle handle, bool isActive);

        void Shutdown();

        template <typename T>
        range<T> GetComponents();

        template <typename T>
        size_t GetComponentCount();

        template <typename T>
        active_range<T> GetActiveComponents();

        range<Entity> GetEntities();

        active_range<Entity> GetActiveEntities();
};

template <typename T>
inline T* HandleManager::GetComponent(ComponentHandle handle)
{
        return reinterpret_cast<T*>(GetData(m_ComponentRandomAccessPools, handle.pool_index, handle.redirection_index));
}

template <typename T>
inline range<T> HandleManager::GetComponents()
{
        NMemory::type_index pool_index = T::SGetTypeIndex();
        if (m_ComponentRandomAccessPools.m_mem_starts.size() <= pool_index)
                return range<T>(0, 0);

        T*     data          = reinterpret_cast<T*>(m_ComponentRandomAccessPools.m_mem_starts[pool_index]);
        size_t element_count = static_cast<size_t>(m_ComponentRandomAccessPools.m_element_counts[pool_index]);
        return range<T>(data, element_count);
}

template <typename T>
inline active_range<T> HandleManager::GetActiveComponents()
{
        NMemory::type_index pool_index = T::SGetTypeIndex();
        if (m_ComponentRandomAccessPools.m_mem_starts.size() <= pool_index)
                return active_range<T>::SGetNullActiveRange();

        T*                       data          = reinterpret_cast<T*>(m_ComponentRandomAccessPools.m_mem_starts[pool_index]);
        size_t                   element_count = static_cast<size_t>(m_ComponentRandomAccessPools.m_element_counts[pool_index]);
        NMemory::dynamic_bitset& isActives     = m_ComponentRandomAccessPools.m_element_isactives[pool_index];
        return active_range<T>(data, element_count, isActives);
}
template <typename T>
inline size_t HandleManager::GetComponentCount()
{
        NMemory::type_index pool_index = T::SGetTypeIndex();
        if (m_ComponentRandomAccessPools.m_mem_starts.size() <= pool_index)
                return 0ULL;
        return static_cast<size_t>(m_ComponentRandomAccessPools.m_element_counts[pool_index]);
}

template <typename T>
inline T* ComponentHandle::Get()
{
        return (handleContext->GetComponent<T>(*this));
}

#include "Entity.h"
template <typename T>
inline ComponentHandle HandleManager::AddComponent(EntityHandle parentHandle)
{
        NMemory::type_index pool_index = T::SGetTypeIndex();
        if (m_ComponentRandomAccessPools.m_mem_starts.size() <= pool_index ||
            m_ComponentRandomAccessPools.m_element_capacities[pool_index] < T::SGetMaxElements())
        {
                if (m_MemoryStack.m_MemCurr + sizeof(T) * T::SGetMaxElements() >= m_MemoryStack.m_MemMax)
                        assert(false);
                InsertPool(
                    m_ComponentRandomAccessPools, {sizeof(T), T::SGetMaxElements()}, m_MemoryStack.m_MemCurr, pool_index);
        }
        auto            allocation = Allocate(m_ComponentRandomAccessPools, pool_index);
        ComponentHandle componentHandle(pool_index, allocation.redirection_idx);
        T*              objectPtr = reinterpret_cast<T*>(allocation.objectPtr);
        new (objectPtr) T();
        objectPtr->m_pool_index               = componentHandle.pool_index;
        objectPtr->m_redirection_index        = componentHandle.redirection_index;
        objectPtr->m_parent_redirection_index = parentHandle.redirection_index;

        Entity*        entities_mem_start = reinterpret_cast<Entity*>(m_EntityRandomAccessPools.m_mem_starts[0]);
        NMemory::index parent_index       = m_EntityRandomAccessPools.m_redirection_indices[0][parentHandle.redirection_index];
        Entity*        parent_mem         = entities_mem_start + parent_index;

        // parent_mem->m_OwnedComponents.emplace(componentHandle.pool_index, componentHandle.redirection_index);
        parent_mem->m_OwnedComponents.push_back({componentHandle.pool_index, componentHandle.redirection_index});

        return componentHandle;
}

template <typename T>
inline std::vector<ComponentHandle> EntityHandle::GetComponents()
{
        NMemory::type_index _type_index = T::SGetTypeIndex();
        // size_t              element_count = this->Get()->m_OwnedComponents.count(_type_index);
        std::vector<ComponentHandle> out;
        auto                         chs = this->Get()->m_OwnedComponents;
        for (auto e : chs)
        {
                if (chs.pool_index == _type_index)
                        out.push_back(e);
        }

        // auto                         kvs = this->Get()->m_OwnedComponents.equal_range(_type_index);
        // std::vector<ComponentHandle> out;
        // out.reserve(element_count);
        // std::for_each(kvs.first, kvs.second, [&](auto e) { out.push_back(ComponentHandle(_type_index, e.second)); });
        return out;
}

template <typename T>
inline ComponentHandle EntityHandle::GetComponentHandle()
{
        NMemory::type_index _type_index = T::SGetTypeIndex();
        // assert(this->Get()->m_OwnedComponents.find(_type_index) != this->Get()->m_OwnedComponents.end()) auto itr =
        //    this->Get()->m_OwnedComponents.find(_type_index);

        // return ComponentHandle(itr->first, itr->second);
        auto chs = this->Get()->m_OwnedComponents;
        for (auto e : chs)
        {
                if (e.pool_index == _type_index)
                        return e;
        }
        assert("component did not exist");
        return ComponentHandle(static_cast<NMemory::type_index>(-1), static_cast<NMemory::index>(-1));
}

template <typename T>
inline T* EntityHandle::GetComponent()
{
        return this->GetComponentHandle<T>().Get<T>();
}

template <typename T>
inline ComponentHandle EntityHandle::AddComponent()
{
        ComponentHandle out = this->handleContext->AddComponent<T>(*this);
        return out;
}
