#pragma once
#include <stdint.h>

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
struct EntityHandle : public Handle
{};
struct ComponentHandle : public Handle
{};