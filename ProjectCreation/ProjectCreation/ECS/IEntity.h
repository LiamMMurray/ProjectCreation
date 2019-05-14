#pragma once
#include <vector>
#include "Component.h"
#include "ComponentManager.h"
#include "ECSTypes.h"
class EntityManager;
class IEntity
{
        EntityManager* m_EntityManager;
        ComponentManager* m_ComponentManager;
    public:
        bool m_Enabled;

        virtual const EntityTypeId GetStaticTypeId() const = 0;

        template <typename T>
        ComponentHandle GetComponentHandle();
        template <typename T>
        T* GetComponent();
        template <typename T>
        std::vector<ComponentHandle> GetComponentHandles();
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
inline T* IEntity::GetComponent()
{
        return nullptr;
}

template <typename T>
inline std::vector<ComponentHandle> IEntity::GetComponentHandles()
{
        return std::vector<ComponentTypeId>();
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
