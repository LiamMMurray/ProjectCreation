#pragma once
#include <vector>
#include "Component.h"
#include "ComponentManager.h"
#include "ECSTypes.h"

class IEntity
{
    public:
        bool m_Enabled;

        virtual const EntityTypeId GetStaticTypeId() const = 0;

        template <typename T>
        ComponentHandle GetComponentHandle();
        template <typename T>
        Component<T>* GetComponent();
        template <typename T>
        std::vector<ComponentHandle> GetComponentHandles();
        template <typename T>
        ComponentIterator<T> GetAllComponents();
        template <typename T>
        EResult CreateComponent(ComponentHandle componentHandle);
        template <typename T>
        void DestroyComponent();
        template <typename T>
        void DestroyComponent(ComponentHandle);
        void Activate();
        void Deactivate();
};

template <typename T>
inline ComponentHandle IEntity::GetComponentHandle()
{
        return ComponentTypeId();
}

template <typename T>
inline Component<T>* IEntity::GetComponent()
{
        return NULL;
}

template <typename T>
inline std::vector<ComponentHandle> IEntity::GetComponentHandles()
{
        return std::vector<ComponentTypeId>();
}

template <typename T>
inline ComponentIterator<T> IEntity::GetAllComponents()
{
        return ComponentIterator<T>();
}

template <typename T>
inline EResult IEntity::CreateComponent(ComponentHandle componentHandle)
{
        return EResult();
}


template <typename T>
inline void IEntity::DestroyComponent()
{}

template <typename T>
inline void IEntity::DestroyComponent(ComponentHandle)
{}
