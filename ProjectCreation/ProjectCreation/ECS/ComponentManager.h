#pragma once
#include "ECSTypes.h"
#include "Handle.h"
#include "IComponent.h"
template <typename T>
class ComponentIterator
{

};
template <typename T>
class ComponentManager
{
    public:
        ComponentManager()
        {}
        T*      GetComponent(ComponentHandle componentHandle);
        EResult CreateComponent(ComponentHandle componentHandle);
        void    DestroyComponent(ComponentHandle componentHandle);
        void    ActivateComponent(ComponentHandle componentHandle);
        void    DeactivateComponent(ComponentHandle componentHandle);
        size_t  GetCapacity();
        size_t  GetSize();
        size_t  GetActiveComponentCount();
        ComponentIterator<T> GetActiveComponents();
};