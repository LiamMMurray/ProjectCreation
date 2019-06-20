#pragma once
#include "IComponent.h"
#include "Memory.h"
#include "../Utility/TypeIndexFactory.h"
#include "ComponentHandle.h"

template <typename T>
class Component : public IComponent
{
    private:
        static const NMemory::type_index s_type_index;
        static NMemory::index            s_max_elements;

    public:
        Component();
        ~Component();
        const NMemory::type_index        GetTypeIndex() const;
        static const NMemory::type_index SGetTypeIndex();
        static void                      SSetMaxElements(NMemory::index max_elements);
        static NMemory::index            SGetMaxElements();
        ComponentHandle                  GetHandle();
};
template <class T>
const NMemory::type_index Component<T>::s_type_index = TypeIndexFactory<IComponent>::GetTypeIndex<T>();
template <class T>
NMemory::index Component<T>::s_max_elements = 5000;

template <typename T>
inline Component<T>::Component()
{}

template <typename T>
inline Component<T>::~Component()
{

}

template <typename T>
inline const NMemory::type_index Component<T>::SGetTypeIndex()
{
        return s_type_index;
}

template <typename T>
inline const NMemory::type_index Component<T>::GetTypeIndex() const
{
        return s_type_index;
}

template <typename T>
inline void Component<T>::SSetMaxElements(NMemory::index max_elements)
{
        s_max_elements = max_elements;
}

template <typename T>
inline NMemory::index Component<T>::SGetMaxElements()
{
        return s_max_elements;
}

template <typename T>
inline ComponentHandle Component<T>::GetHandle()
{
        return ComponentHandle(m_pool_index, m_redirection_index);
}
