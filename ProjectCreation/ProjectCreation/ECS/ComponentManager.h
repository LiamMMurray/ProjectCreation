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
        EResult CreateComponent(ComponentHandle componentHandle);
        void    DestroyComponent(ComponentHandle componentHandle);
        void    ActivateComponent(ComponentHandle componentHandle);
        void    DeactivateComponent(ComponentHandle componentHandle);
        size_t  GetCapacity();
        size_t  GetSize();
        size_t  GetActiveComponentCount();
        template <typename T>
        T*      GetComponent(ComponentHandle componentHandle);
        template <typename T>
        ComponentIterator<T> GetActiveComponents();
};

template <typename T>
inline T* ComponentManager::GetComponent(ComponentHandle componentHandle)
{
        return NULL;
}

template <typename T>
inline ComponentIterator<T> ComponentManager::GetActiveComponents()
{
        return ComponentIterator<T>();
}
