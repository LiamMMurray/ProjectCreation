#pragma once
#include <vector>
#include "Component.h"
#include "ComponentManager.h"
#include "ECSTypes.h"
#include "Handle.h"

class IEntity
{
  //  public:
		//virtual static const TypeId<IEntity> GetTypeId() const = 0;

    public:
        template <typename T>
        Handle<IComponent> GetComponentHandle();
        template <typename T>
        Component<T>* GetComponent();
        template <typename T>
        std::vector<Handle<IComponent>> GetComponentHandles();
        template <typename T>
        ComponentIterator<T> GetAllComponents();
        template <typename T>
        EResult CreateComponent(Handle<IComponent> componentHandle);
        template <typename T>
        void DestroyComponent();
        template <typename T>
        void DestroyComponent(Handle<IComponent>);
        void Activate();
        void Deactivate();
};

template <typename T>
inline Handle<IComponent> IEntity::GetComponentHandle()
{
        return TypeId<IComponent>();
}

template <typename T>
inline Component<T>* IEntity::GetComponent()
{
        return NULL;
}

template <typename T>
inline std::vector<Handle<IComponent>> IEntity::GetComponentHandles()
{
        return std::vector<TypeId<IComponent>>();
}

template <typename T>
inline ComponentIterator<T> IEntity::GetAllComponents()
{
        return ComponentIterator<T>();
}

template <typename T>
inline EResult IEntity::CreateComponent(Handle<IComponent> componentHandle)
{
        return EResult();
}


template <typename T>
inline void IEntity::DestroyComponent()
{}

template <typename T>
inline void IEntity::DestroyComponent(Handle<IComponent>)
{}
