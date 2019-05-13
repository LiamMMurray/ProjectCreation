#pragma once
#include <unordered_map>
#include <vector>
#include "../ErrorHandling/ErrorTypes.h"
#include "Component.h"
#include "ECSTypes.h"
#include "HandleManager.h"

template <typename T>
class ComponentIterator
{};

class ComponentManager
{
    private:
        typedef std::unordered_map<ComponentTypeId, ComponentHandle>       ComponentTypeIdHandleMap;
        typedef std::unordered_map<EntityHandle, ComponentTypeIdHandleMap> EntityComponentIdMap;

        HandleManager<IComponent> m_HandleManager;
        EntityComponentIdMap      m_EntityComponentIdMap;


    public:
        ComponentManager()
        {}
        template <typename T>
        ComponentHandle AddComponent(const EntityHandle entityHandle);
        template <class T>
        T*              GetComponent(const EntityHandle entityHandle);
        void            ActivateComponent(ComponentHandle componentHandle);
        void            DeactivateComponent(ComponentHandle componentHandle);
        ComponentHandle GetComponentHandle(EntityHandle entityHandle, ComponentTypeId componentTypeId);
        IComponent*     GetComponent(ComponentHandle componentHandle);

        // Non-implemented
        //////////////////////////////////////
        //////////////////////////////////////
        //////////////////////////////////////
        template <typename T>
        ComponentIterator<T> GetActiveComponents();
        EResult              CreateComponent(ComponentHandle componentHandle);
        void                 DestroyComponent(ComponentHandle componentHandle);
        size_t               GetCapacity();
        size_t               GetSize();
        size_t               GetActiveComponentCount();
        //////////////////////////////////////
        //////////////////////////////////////
        //////////////////////////////////////
};

template <typename T>
inline ComponentHandle ComponentManager::AddComponent(const EntityHandle entityHandle)
{
        static_assert(std::is_base_of<Component<T>, T>::value,
                      "Addcomponent can only accept CRTP classes that inherit from Component<T>");
        T*              pComponent      = new T();
        ComponentHandle componentHandle = m_HandleManager.GetHandle(pComponent);
        pComponent->m_Owner             = entityHandle;
        ComponentTypeId componentTypeId = pComponent->GetTypeId();
        // TODO
        // highly convenient but also highly unoptimized (requires at minimum TWO hashes for every time this gets called)
        m_EntityComponentIdMap[entityHandle][componentTypeId] = componentHandle;
        return componentHandle;
}

template <class T>
inline T* ComponentManager::GetComponent(const EntityHandle entityHandle)
{
        ComponentTypeId componentTypeId = T::GetTypeId();
        ComponentHandle componentHandle = GetComponentHandle(entityHandle, componentTypeId);
        if (componentHandle.IsValid())
                return nullptr;
        else
                return (T*)m_HandleManager.GetObject(componentHandle);
}

template <typename T>
inline ComponentIterator<T> ComponentManager::GetActiveComponents()
{
        return ComponentIterator<T>();
}
