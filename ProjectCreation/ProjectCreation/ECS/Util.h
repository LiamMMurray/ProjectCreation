#pragma once
#include "ECSTypes.h"

template <typename T>
class TypeIdCreator
{
        static ITypeId m_Count;

    public:
        template <typename U>
        static ITypeId GetUniqueTypeId()
        {
                static const ITypeId increment_once_per_secondary_type{m_Count++};
                return increment_once_per_secondary_type;
        }
};
template <typename T>
ITypeId TypeIdCreator<T>::m_Count{0}; 