#pragma once
#include <stdint.h>

template <typename T>
struct Handle
{
        union
        {
                uint64_t m_Data;
                struct
                {
                        uint32_t m_Id;
                        uint32_t m_DeletionAccumulator;
                };
        };
};
