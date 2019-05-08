#pragma once
template <typename T>
class TComponentIterator
{};

template <typename T>
class TComponentContainer
{
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

