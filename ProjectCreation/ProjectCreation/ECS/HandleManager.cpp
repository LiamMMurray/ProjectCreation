#include "HandleManager.h"
#include "Entity.h"
#include "IComponent.h"

HandleManager::HandleManager(NMemory::NPools::RandomAccessPools& componentRandomAccessPools,
                             NMemory::NPools::RandomAccessPools& entityRandomAccessPools,
                             NMemory::MemoryStack&                poolMemory) :
    m_ComponentRandomAccessPools(componentRandomAccessPools),
    m_EntityRandomAccessPools(entityRandomAccessPools),
    m_MemoryStack(poolMemory),
    m_PoolCount(TypeIndexFactory<IComponent>::GetTypeIndex<void>())
// TypeIndexFactory<IComponent>::GetTypeIndex<void>() gets one past the last pool's index since this is the only place this
// function is called dynamically, and not statically.
{
        ComponentHandle::handleContext = this;
        EntityHandle::handleContext    = this;
}

HandleManager::~HandleManager()
{
        Shutdown();
}

Entity* HandleManager::GetEntity(EntityHandle handle)
{
        return reinterpret_cast<Entity*>(GetData(m_EntityRandomAccessPools, 0, handle.redirection_index));
}

EntityHandle HandleManager::CreateEntity(EntityHandle parentHandle)
{
        NMemory::type_index pool_index = 0;
        if (m_EntityRandomAccessPools.m_mem_starts.size() <= pool_index)
        {
                assert(m_MemoryStack.m_MemCurr + sizeof(Entity) * Entity::SGetMaxElements() <= m_MemoryStack.m_MemMax);
                InsertPool(m_EntityRandomAccessPools, {sizeof(Entity), Entity::SGetMaxElements()}, m_MemoryStack.m_MemCurr, pool_index);
        }
        auto         allocation   = Allocate(m_EntityRandomAccessPools, pool_index);
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
        Free(m_ComponentRandomAccessPools, handle.pool_index, _adapter);
}

void HandleManager::FreeEntity(EntityHandle handle)
{
        // handle.Get()->~Entity();
        NMemory::indices _adapter = {{handle.redirection_index}};
        Free(m_EntityRandomAccessPools, 0, _adapter);
}

void HandleManager::ReleaseComponentHandle(ComponentHandle handle)
{
        NMemory::indices _adapter = {{handle.redirection_index}};
        ReleaseRedirectionIndices(m_ComponentRandomAccessPools, handle.pool_index, _adapter);
}

void HandleManager::ReleaseEntityHandle(EntityHandle handle)
{
        NMemory::indices _adapter = {{handle.redirection_index}};
        ReleaseRedirectionIndices(m_EntityRandomAccessPools, 0, _adapter);
}

bool HandleManager::IsActive(ComponentHandle handle)
{
        NMemory::index element_index =
            m_ComponentRandomAccessPools.m_redirection_indices[handle.pool_index][handle.redirection_index];
        if (element_index == -1)
                return false;
        return m_ComponentRandomAccessPools.m_element_isactives[handle.pool_index][element_index];
}

bool HandleManager::IsActive(EntityHandle handle)
{
        NMemory::index element_index = m_ComponentRandomAccessPools.m_redirection_indices[0][handle.redirection_index];
        if (element_index == -1)
                return false;
        return m_EntityRandomAccessPools.m_element_isactives[0][element_index];
}

void HandleManager::SetIsActive(ComponentHandle handle, bool isActive)
{
        NMemory::index element_index =
            m_ComponentRandomAccessPools.m_redirection_indices[handle.pool_index][handle.redirection_index];
        if (element_index == -1)
                return;
        m_ComponentRandomAccessPools.m_element_isactives[handle.pool_index][element_index] = isActive;
}

void HandleManager::SetIsActive(EntityHandle handle, bool isActive)
{
        NMemory::index element_index = m_ComponentRandomAccessPools.m_redirection_indices[0][handle.redirection_index];
        if (element_index == -1)
                return;
        m_EntityRandomAccessPools.m_element_isactives[0][element_index] = isActive;
}

void HandleManager::Shutdown()
{
        NMemory::NPools::ClearPools(m_ComponentRandomAccessPools);
        NMemory::NPools::ClearPools(m_EntityRandomAccessPools);
}

range<Entity> HandleManager::GetEntities()
{
        if (m_EntityRandomAccessPools.m_mem_starts.size() == 0)
                return range<Entity>(0, 0);

        Entity* data          = reinterpret_cast<Entity*>(m_EntityRandomAccessPools.m_mem_starts[0]);
        size_t  element_count = static_cast<size_t>(m_EntityRandomAccessPools.m_element_counts[0]);
        return range<Entity>(data, element_count);
}

active_range<Entity> HandleManager::GetActiveEntities()
{
        if (m_EntityRandomAccessPools.m_mem_starts.size() == 0)
                return active_range<Entity>::SGetNullActiveRange();

        Entity*                  data          = reinterpret_cast<Entity*>(m_EntityRandomAccessPools.m_mem_starts[0]);
        size_t                   element_count = static_cast<size_t>(m_EntityRandomAccessPools.m_element_counts[0]);
        NMemory::dynamic_bitset& isActives     = m_EntityRandomAccessPools.m_element_isactives[0];
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