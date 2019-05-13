#pragma once
#include <stdint.h>

template <typename T>
struct Handle
{
        Handle() : m_Data(0)
        {}
        Handle(uint32_t id, uint32_t version) : m_Id(id), m_DeletionAccumulator(version)
        {}
        union
        {
                uint64_t m_Data;
                struct
                {
                        uint32_t m_Id;
                        uint32_t m_DeletionAccumulator;
                };
        };
		bool operator==(const Handle& other) const
		{
                return this->m_Data == other.m_Data;
		}
};
