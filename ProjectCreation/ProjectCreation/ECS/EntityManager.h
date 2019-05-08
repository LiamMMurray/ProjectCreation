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