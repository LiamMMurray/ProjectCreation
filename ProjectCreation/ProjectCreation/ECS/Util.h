#pragma once
#include "ECSTypes.h"

template <typename T>
class TypeIdCreator
{
        static TypeId m_Count;

    public:
        template <typename U>
        static TypeId GetUniqueTypeId()
        {
                static const TypeId increment_once_per_secondary_type{m_Count++};
                return increment_once_per_secondary_type;
        }
};
template <typename T>
TypeId TypeIdCreator<T>::m_Count{0};