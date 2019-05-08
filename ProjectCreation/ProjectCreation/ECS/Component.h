#pragma once
#include "IComponent.h"
#include "Util.h"
void IComponent::Enable()
{
        m_Enabled = true;
}
void IComponent::Disable()
{
        m_Enabled = false;
}
bool isEnabled();

template <typename T>
class Component
{
    public:
        static const TypeId m_TypeId;
};
template <class T>
const TypeId Component<T>::m_TypeId = TypeIdCreator<IComponent>::GetUniqueTypeId<T>();