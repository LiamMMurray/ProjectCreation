#pragma once
#include "IComponent.h"
#include "Util.h"

template <typename T>
class Component : public IComponent
{
    private:
        static const ComponentTypeId m_TypeId;

    public:
        Component();
       void Init();
        static const ComponentTypeId GetTypeId();
        const ComponentTypeId        GetStaticTypeId() const;
};
template <class T>
const ComponentTypeId Component<T>::m_TypeId = TypeIdCreator<IComponent>::GetUniqueTypeId<T>();

template <typename T>
inline Component<T>::Component()
{
        static_assert(std::is_base_of<Component<T>, T>::value, "Components must derive from Component<T>");
}

template <typename T>
inline void Component<T>::Init()
{
        static_cast<T*>(this)->Init();
}

template <typename T>
inline const ComponentTypeId Component<T>::GetTypeId()
{
        static_assert(std::is_base_of<Component<T>, T>::value, "Components must follow derive from Component<T>");
        return m_TypeId;
}

template <typename T>
inline const ComponentTypeId Component<T>::GetStaticTypeId() const
{
        return m_TypeId;
}