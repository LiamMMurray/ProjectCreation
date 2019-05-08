#pragma once
#include "Component.h"
template <typename T>
class TComponentIterator
{};

template <typename T>
class TComponentContainer
{
    public:
        IComponent*           GetComponent(ComponentHandle componentHandle);
        bool                  CreateComponent(ComponentHandle componentHandle, EntityHandle entityHandle);
        void                  DestroyComponent(ComponentHandle componentHandle);
        void                  ActivateComponent(ComponentHandle componentHandle);
        void                  DeactivateComponent(ComponentHandle componentHandle);
        size_t                GetContainerCapacity();
        size_t                GetContainerSize();
        size_t                GetContainerActiveCount();
        TComponentIterator<T> GetActiveComponents();
};

template <typename T>
inline IComponent* TComponentContainer<T>::GetComponent(ComponentHandle componentHandle)
{
        return NULL;
}

template <typename T>
inline bool TComponentContainer<T>::CreateComponent(ComponentHandle componentHandle, EntityHandle entityHandle)
{
        return false;
}

template <typename T>
inline void TComponentContainer<T>::DestroyComponent(ComponentHandle componentHandle)
{}

template <typename T>
inline void TComponentContainer<T>::ActivateComponent(ComponentHandle componentHandle)
{}

template <typename T>
inline void TComponentContainer<T>::DeactivateComponent(ComponentHandle componentHandle)
{}

template <typename T>
inline size_t TComponentContainer<T>::GetContainerCapacity()
{
        return size_t();
}

template <typename T>
inline size_t TComponentContainer<T>::GetContainerSize()
{
        return size_t();
}

template <typename T>
inline size_t TComponentContainer<T>::GetContainerActiveCount()
{
        return size_t();
}

template <typename T>
inline TComponentIterator<T> TComponentContainer<T>::GetActiveComponents()
{
        return TComponentIterator<T>();
}
