#pragma once
#include "Component.h"
#include "IEntity.h"
template <typename T>
class TComponentIterator
{};

template <typename T>
class TComponentContainer
{
    public:
        IComponent*           GetComponent(Handle<IComponent> componentHandle);
        bool                  CreateComponent(Handle<IComponent> componentHandle, Handle<IEntity> entityHandle);
        void                  DestroyComponent(Handle<IComponent> componentHandle);
        void                  ActivateComponent(Handle<IComponent> componentHandle);
        void                  DeactivateComponent(Handle<IComponent> componentHandle);
        size_t                GetContainerCapacity();
        size_t                GetContainerSize();
        size_t                GetContainerActiveCount();
        TComponentIterator<T> GetActiveComponents();
};

template <typename T>
inline IComponent* TComponentContainer<T>::GetComponent(Handle<IComponent> componentHandle)
{
        return NULL;
}

template <typename T>
inline bool TComponentContainer<T>::CreateComponent(Handle<IComponent> componentHandle, Handle<IEntity> entityHandle)
{
        return false;
}

template <typename T>
inline void TComponentContainer<T>::DestroyComponent(Handle<IComponent> componentHandle)
{}

template <typename T>
inline void TComponentContainer<T>::ActivateComponent(Handle<IComponent> componentHandle)
{}

template <typename T>
inline void TComponentContainer<T>::DeactivateComponent(Handle<IComponent> componentHandle)
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
