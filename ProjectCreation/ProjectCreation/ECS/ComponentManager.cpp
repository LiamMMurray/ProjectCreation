#include "ComponentManager.h"

ComponentHandle ComponentManager::GetComponentHandle(EntityHandle entityHandle, ComponentTypeId componentTypeId)
{
        if (m_EntityComponentIdMap.find(entityHandle) != m_EntityComponentIdMap.end() &&
            m_EntityComponentIdMap[entityHandle].find(componentTypeId) != m_EntityComponentIdMap[entityHandle].end())
        {
                return m_EntityComponentIdMap[entityHandle][componentTypeId];
        }
        ComponentHandle out;
        out.SetInvalid();
        return out;
}

// IComponent* ComponentManager::GetComponent(ComponentHandle componentHandle)
//{
//        return m_HandleManager.GetObject(componentHandle);
//}

// void ComponentManager::DestroyComponent(ComponentHandle componentHandle)
//{}

void ComponentManager::ActivateComponent(ComponentHandle componentHandle)
{
        ComponentTypeId typeId = m_TypeIdMap[componentHandle];
        m_TypeAssociativeHandleManagers[typeId.m_Data].GetObject(componentHandle)->Enable();
}

void ComponentManager::DeactivateComponent(ComponentHandle componentHandle)
{
        ComponentTypeId typeId = m_TypeIdMap[componentHandle];
        m_TypeAssociativeHandleManagers[typeId.m_Data].GetObject(componentHandle)->Disable();
}

size_t ComponentManager::GetCapacity()
{
        /*      return m_HandleManager.*/
        return 0;
}


size_t ComponentManager::GetSize()
{
        return 0;
}

size_t ComponentManager::GetActiveComponentCount()
{
        /*    return m_HandleManager.GetSize();*/
        return 0;
}
