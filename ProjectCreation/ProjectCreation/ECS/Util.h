#pragma once
#include "ECSTypes.h"
#include <vector>


template <typename T>
class TypeUtility
{
        static uint32_t m_Count;
    public:
        template <typename SubType>
        static uint32_t GetUniqueTypeId()
        {
                static const uint32_t increment_once_per_secondary_type{m_Count++};
                return increment_once_per_secondary_type;
        }
};
template <typename T>
uint32_t TypeUtility<T>::m_Count{0};
