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
                static_assert(std::is_base_of<IComponent, T>::value || std::is_base_of<IEntity, T>::value,
                              "Error. Template type must be subclass of IComponent or IEntity");

                static const uint32_t increment_once_per_secondary_type{m_Count++};
                return increment_once_per_secondary_type;
        }
};
template <typename T>
uint32_t TypeIdCreator<T>::m_Count{0};

