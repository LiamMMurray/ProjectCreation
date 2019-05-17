#pragma once
#include <assert.h>
#include <unordered_map>
#include <vector>
#include "../ErrorHandling/ErrorTypes.h"
#include "Component.h"
#include "ECSTypes.h"
#include "HandleManager.h"


class ComponentManager
{

        // TODO make friend class relationship
    public:
        typedef std::unordered_map<ComponentTypeId, ComponentHandle>       ComponentTypeIdHandleMap;
        typedef std::unordered_map<EntityHandle, ComponentTypeIdHandleMap> EntityComponentIdMap;
        typedef std::vector<HandleManager<IComponent>>                     TypeAssociativeHandleManagers;
        typedef std::unordered_map<ComponentHandle, ComponentTypeId>       TypeIdLookUpTable;

        HandleManager<IComponent>     m_HandleManager;
        TypeAssociativeHandleManagers m_TypeAssociativeHandleManagers;
        EntityComponentIdMap          m_EntityComponentIdMap;
        TypeIdLookUpTable             m_TypeIdMap;

    public:
        ComponentManager()
        {}
        template <typename T>
        ComponentHandle AddComponent(const EntityHandle entityHandle);
        template <class T>
        T* GetComponent(const EntityHandle entityHandle);
        template <class T>
        T* GetComponent(const ComponentHandle componentHandle);
        template <typename T>
        HandleManager<IComponent>& GetActiveComponents();
        void                       ActivateComponent(ComponentHandle componentHandle);
        void                       DeactivateComponent(ComponentHandle componentHandle);
        ComponentHandle            GetComponentHandle(EntityHandle entityHandle, ComponentTypeId componentTypeId);

        size_t GetSize();


        // Non-implemented
        //////////////////////////////////////
        //////////////////////////////////////
        //////////////////////////////////////
        // EResult CreateComponent(ComponentHandle componentHandle);
        void DestroyComponent(ComponentHandle componentHandle);
        // IComponent*     GetComponent(ComponentHandle componentHandle);
        size_t GetCapacity();
        size_t GetActiveComponentCount();
        //////////////////////////////////////
        //////////////////////////////////////
        //////////////////////////////////////
};

template <typename T>
inline ComponentHandle ComponentManager::AddComponent(const EntityHandle entityHandle)
{
        static_assert(std::is_base_of<Component<T>, T>::value,
                      "Addcomponent can only accept CRTP classes that inherit from Component<T>");
        T* pComponent                   = new T();
        pComponent->m_Owner             = entityHandle;
        ComponentTypeId componentTypeId = pComponent->GetTypeId();

        if (componentTypeId.m_Data >= m_TypeAssociativeHandleManagers.size())
        {
                m_TypeAssociativeHandleManagers.resize(componentTypeId.m_Data + 1);
        }
        ComponentHandle componentHandle = m_TypeAssociativeHandleManagers[componentTypeId.m_Data].GetHandle(pComponent);
        // TODO
        // highly convenient but also highly unoptimized (requires at minimum TWO hashes for every time this gets called)
        m_EntityComponentIdMap[entityHandle][componentTypeId] = componentHandle;
        pComponent->m_Handle                                  = componentHandle;
        m_TypeIdMap[componentHandle]                          = componentTypeId;
        return componentHandle;
}

template <class T>
inline T* ComponentManager::GetComponent(const EntityHandle entityHandle)
{
        ComponentTypeId componentTypeId = T::GetTypeId();
        ComponentHandle componentHandle = GetComponentHandle(entityHandle, componentTypeId);
        if (!componentHandle.IsValid())
                return nullptr;
        else
                return (T*)m_TypeAssociativeHandleManagers[componentTypeId.m_Data].GetObject(componentHandle);
}

template <class T>
inline T* ComponentManager::GetComponent(const ComponentHandle componentHandle)
{
        ComponentTypeId componentTypeId = T::GetTypeId();
        return (T*)m_TypeAssociativeHandleManagers[componentTypeId.m_Data].GetObject(componentHandle);
}

template <typename T>
inline HandleManager<IComponent>& ComponentManager::GetActiveComponents()
{
        ComponentTypeId componentTypeId = T::GetTypeId();
        assert(componentTypeId.m_Data < m_TypeAssociativeHandleManagers.size());
        return m_TypeAssociativeHandleManagers[componentTypeId.m_Data];
}