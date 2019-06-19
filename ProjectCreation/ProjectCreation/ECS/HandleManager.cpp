#include "HandleManager.h"
#include "Entity.h"
#include "IComponent.h"

HandleManager::HandleManager(NMemory::NPools::RandomAccessPools& componentRandomAccessPools,
                             NMemory::NPools::RandomAccessPools& entityRandomAccessPools,
                             NMemory::PoolMemory&                poolMemory) :
    component_random_access_pools(componentRandomAccessPools),
    entity_random_access_pools(entityRandomAccessPools),
    pool_memory(poolMemory),
    pool_count(TypeIndexFactory<IComponent>::GetTypeIndex<void>())
// TypeIndexFactory<IComponent>::GetTypeIndex<void>() gets one past the last pool's index since this is the only place this
// function is called dynamically, and not statically.
{
        ComponentHandle::handleContext = this;
        EntityHandle::handleContext    = this;
}

HandleManager::~HandleManager()
{
        ShutDown();
}

Entity* HandleManager::GetEntity(EntityHandle handle)
{
        return reinterpret_cast<Entity*>(GetData(entity_random_access_pools, 0, handle.redirection_index));
}

EntityHandle HandleManager::CreateEntity(EntityHandle parentHandle)
{
        NMemory::type_index pool_index = 0;
        if (entity_random_access_pools.m_mem_starts.size() <= pool_index)
        {
                assert(pool_memory.m_MemCurr + sizeof(Entity) * Entity::SGetMaxElements() <= pool_memory.m_MemMax);
                InsertPool(entity_random_access_pools, {sizeof(Entity), Entity::SGetMaxElements()}, pool_memory.m_MemCurr, pool_index);
        }
        auto         allocation   = Allocate(entity_random_access_pools, pool_index);
        EntityHandle entityHandle = allocation.redirection_idx;
        Entity*      objectPtr    = reinterpret_cast<Entity*>(allocation.objectPtr);
        new (objectPtr) Entity();
        objectPtr->m_redirection_index        = allocation.redirection_idx;
        objectPtr->m_parent_redirection_index = parentHandle.redirection_index;

        return entityHandle;
}

void HandleManager::FreeComponent(ComponentHandle handle)
{
        NMemory::indices _adapter = {{handle.redirection_index}};
        Free(component_random_access_pools, handle.pool_index, _adapter);
}

void HandleManager::FreeEntity(EntityHandle handle)
{
        // handle.Get()->~Entity();
        NMemory::indices _adapter = {{handle.redirection_index}};
        Free(entity_random_access_pools, 0, _adapter);
}

void HandleManager::ReleaseComponentHandle(ComponentHandle handle)
{
        NMemory::indices _adapter = {{handle.redirection_index}};
        ReleaseRedirectionIndices(component_random_access_pools, handle.pool_index, _adapter);
}

void HandleManager::ReleaseEntityHandle(EntityHandle handle)
{
        NMemory::indices _adapter = {{handle.redirection_index}};
        ReleaseRedirectionIndices(entity_random_access_pools, 0, _adapter);
}

bool HandleManager::IsActive(ComponentHandle handle)
{
        NMemory::index element_index =
            component_random_access_pools.m_redirection_indices[handle.pool_index][handle.redirection_index];
        if (element_index == -1)
                return false;
        return component_random_access_pools.m_element_isactives[handle.pool_index][element_index];
}

bool HandleManager::IsActive(EntityHandle handle)
{
        NMemory::index element_index = component_random_access_pools.m_redirection_indices[0][handle.redirection_index];
        if (element_index == -1)
                return false;
        return entity_random_access_pools.m_element_isactives[0][element_index];
}

void HandleManager::SetIsActive(ComponentHandle handle, bool isActive)
{
        NMemory::index element_index =
            component_random_access_pools.m_redirection_indices[handle.pool_index][handle.redirection_index];
        if (element_index == -1)
                return;
        component_random_access_pools.m_element_isactives[handle.pool_index][element_index] = isActive;
}

void HandleManager::SetIsActive(EntityHandle handle, bool isActive)
{
        NMemory::index element_index = component_random_access_pools.m_redirection_indices[0][handle.redirection_index];
        if (element_index == -1)
                return;
        entity_random_access_pools.m_element_isactives[0][element_index] = isActive;
}

void HandleManager::ShutDown()
{
        NMemory::NPools::ClearPools(component_random_access_pools);
        NMemory::NPools::ClearPools(entity_random_access_pools);
}

range<Entity> HandleManager::GetEntities()
{
        if (entity_random_access_pools.m_mem_starts.size() == 0)
                return range<Entity>(0, 0);

        Entity* data          = reinterpret_cast<Entity*>(entity_random_access_pools.m_mem_starts[0]);
        size_t  element_count = static_cast<size_t>(entity_random_access_pools.m_element_counts[0]);
        return range<Entity>(data, element_count);
}

active_range<Entity> HandleManager::GetActiveEntities()
{
        if (entity_random_access_pools.m_mem_starts.size() == 0)
                return active_range<Entity>::GetNullActiveRange();

        Entity*                  data          = reinterpret_cast<Entity*>(entity_random_access_pools.m_mem_starts[0]);
        size_t                   element_count = static_cast<size_t>(entity_random_access_pools.m_element_counts[0]);
        NMemory::dynamic_bitset& isActives     = entity_random_access_pools.m_element_isactives[0];
        return active_range<Entity>(data, element_count, isActives);
}

ComponentHandle::ComponentHandle(NMemory::type_index pool_index, NMemory::index redirection_index) :
    pool_index(pool_index),
    redirection_index(redirection_index)
{}

ComponentHandle::ComponentHandle() : pool_index(0), redirection_index(0)
{}

void ComponentHandle::Free()
{
        // TODO this MUST be deffered
        handleContext->FreeComponent(*this);
        handleContext->ReleaseComponentHandle(*this);
}

bool ComponentHandle::IsActive()
{
        return handleContext->IsActive(*this);
}

void ComponentHandle::SetIsActive(bool isActive)
{
        handleContext->SetIsActive(*this, isActive);
}

bool ComponentHandle::operator==(const ComponentHandle& other) const
{
        return other.pool_index == this->pool_index && other.redirection_index == this->redirection_index;
}

EntityHandle::EntityHandle(NMemory::index redirection_index) : redirection_index(redirection_index)
{}

EntityHandle::EntityHandle() : redirection_index(-1)
{}

Entity* EntityHandle::Get()
{
        return (handleContext->GetEntity(*this));
}

void EntityHandle::Free()
{
        this->FreeComponents();
        handleContext->FreeEntity(*this);
        handleContext->ReleaseEntityHandle(*this);
}

bool EntityHandle::IsActive()
{
        return handleContext->IsActive(*this);
}

void EntityHandle::SetIsActive(bool isActive)
{
        handleContext->SetIsActive(*this, isActive);
}

bool EntityHandle::operator==(const EntityHandle& other) const
{
        return this->redirection_index == other.redirection_index;
}

void EntityHandle::FreeComponents()
{
        for (auto& e : this->Get()->m_OwnedComponents)
        {
                ComponentHandle(e.first, e.second).Free();
        }
}

HandleManager* ComponentHandle::handleContext = 0;
HandleManager* EntityHandle::handleContext    = 0;