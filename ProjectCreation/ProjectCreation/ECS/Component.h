#pragma once
#include "IComponent.h"
#include "Util.h"

template <typename T>
class Component
{
    private:
        static const TypeId<IComponent> m_TypeId;

    public:
        static const TypeId<IComponent> GetTypeId();
};
template <class T>
const TypeId<IComponent> Component<T>::m_TypeId = TypeIdCreator<IComponent>::GetUniqueTypeId<T>();

template <typename T>
inline const TypeId<IComponent> Component<T>::GetTypeId()
{
        return m_TypeId;
}