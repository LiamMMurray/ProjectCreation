#pragma once
#include "ECSTypes.h"
#include "Entity.h"
#include "Component.h"
class EntityManager
{
    public:
        template <typename T>
        Entity<T>* GetEntity();
        template <typename T>
        EResult CreateEntity(EntityHandle entityHandle);
        template <typename T>
        void DestroyEntity(EntityHandle entityHandle);
		template <typename T>
        void ActivateEntity(EntityHandle entityHandle);
        template <typename T>
        void DeactivateEntity(EntityHandle entityHandle);
		template <typename T>
        size_t GetContainerSize();
		template <typename entityType>
        size_t GetContainerActiveCount();
		template <typename T>
        Component<T>* GetActiveComponents();
};

template <typename T>
inline Entity<T>* EntityManager::GetEntity()
{
        return NULL;
}

template <typename T>
inline EResult EntityManager::CreateEntity(EntityHandle entityHandle)
{
        return EResult();
}

template <typename T>
inline void EntityManager::DestroyEntity(EntityHandle entityHandle)
{}

template <typename T>
inline void EntityManager::ActivateEntity(EntityHandle entityHandle)
{}

template <typename T>
inline void EntityManager::DeactivateEntity(EntityHandle entityHandle)
{}

template <typename T>
inline size_t EntityManager::GetContainerSize()
{
        return size_t();
}

template <typename entityType>
inline size_t EntityManager::GetContainerActiveCount()
{
        return size_t();
}

template <typename T>
inline Component<T>* EntityManager::GetActiveComponents()
{
        return NULL;
}
