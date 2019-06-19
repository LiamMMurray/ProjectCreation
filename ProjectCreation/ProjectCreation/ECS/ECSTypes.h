#pragma once
#include <stdint.h>
#include <functional>
#include "Handle.h"

template <typename T>
struct ECSTypeId
{
        uint32_t m_Data;

        ECSTypeId() : m_Data(0)
        {}

        ECSTypeId(uint32_t data) : m_Data(data)
        {}

        ECSTypeId operator++(int)
        {
                ECSTypeId temp(*this);
                m_Data++;
                return temp;
        }
        bool operator==(const ECSTypeId<T>& other) const
        {
                return this->m_Data == other.m_Data;
        }
		bool operator!=(const ECSTypeId<T>& other) const
		{
                return this->m_Data != other.m_Data;
		}
};

template <typename T>
class std::hash<ECSTypeId<T>>
{
    public:
        size_t operator()(const ECSTypeId<T> id) const
        {
                return hash<uint32_t>()(id.m_Data);
        }
};

class ISystem;
typedef ECSTypeId<ISystem>    SystemTypeId;