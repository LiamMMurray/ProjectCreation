#pragma once
#include "ECSTypes.h"
#include "Handle.h"
#include "IComponent.h"
template <typename T>
class ComponentIterator
{

};

class ComponentManager
{
    public:
        ComponentManager()
        {}
        EResult CreateComponent(Handle<IComponent> componentHandle);
        void    DestroyComponent(Handle<IComponent> componentHandle);
        void    ActivateComponent(Handle<IComponent> componentHandle);
        void    DeactivateComponent(Handle<IComponent> componentHandle);
        size_t  GetCapacity();
        size_t  GetSize();
        size_t  GetActiveComponentCount();
        template <typename T>
        T* GetComponent(Handle<IComponent> componentHandle);
        template <typename T>
        ComponentIterator<T> GetActiveComponents();
};

template <typename T>
inline T* ComponentManager::GetComponent(Handle<IComponent> componentHandle)
{
        return NULL;
}

template <typename T>
inline ComponentIterator<T> ComponentManager::GetActiveComponents()
{
        return ComponentIterator<T>();
}
