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
enum ERESULT_FLAG
{
        SUCCESS      = 0b00000000,
        PLACEHOLDER1 = 0b00000001,
        PLACEHOLDER2 = 0b00000010,
        PLACEHOLDER3 = 0b00000100,
        PLACEHOLDER4 = 0b00001000,
        PLACEHOLDER5 = 0b00010000,
        PLACEHOLDER6 = 0b00100000,
        PLACEHOLDER7 = 0b01000000,
        PLACEHOLDER8 = 0b10000000
};
struct EResult
{
        uint64_t m_Flags;
};