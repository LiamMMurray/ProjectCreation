#pragma once
#include <stdint.h>
struct TypeId
{
        uint32_t m_Data;

        TypeId operator++(int)
        {
                TypeId temp(*this);
                m_Data++;
                return temp;
        }
};
