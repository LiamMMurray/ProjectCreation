#pragma once
#include "ECSTypes.h"

template <typename T>
class TypeIdCreator
{
        static uint32_t m_Count;

    public:
        template <typename U>
        static uint32_t GetUniqueTypeId()
        {
                static const uint32_t increment_once_per_secondary_type{m_Count++};
                return increment_once_per_secondary_type;
        }
};
template <typename T>
uint32_t TypeIdCreator<T>::m_Count{0};
