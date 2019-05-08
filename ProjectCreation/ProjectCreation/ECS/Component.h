#pragma once
#include "IComponent.h"
#include "Util.h"

template <typename T>
class Component
{
    public:
        static const ComponentTypeId m_TypeId;
};
template <class T>
const ComponentTypeId Component<T>::m_TypeId = TypeIdCreator<IComponent>::GetUniqueTypeId<T>();