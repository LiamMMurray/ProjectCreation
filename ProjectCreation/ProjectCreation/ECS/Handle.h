#pragma once
#include <stdint.h>
#include "../ErrorHandling/ErrorTypes.h"
template <typename T>
struct Handle
{
        Handle() : m_Data(0)
        {}
        Handle(uint32_t id, uint32_t version = 0U, uint8_t flags = 0U) :
            m_Id(id),
            m_DeletionAccumulator(version),
            m_Flags(flags)
        {}
        union
        {
                uint64_t m_Data;
                struct
                {
                        uint32_t m_Id;
                        uint32_t m_DeletionAccumulator : 24;
                        uint8_t  m_Flags : 8;
                };
        };
        bool operator==(const Handle<T>& other) const
        {
                return this->m_Data == other.m_Data;
        }
        bool IsValid()
        {
                return (m_Flags & ERESULT_FLAG::INVALID == 0);
        }
        void SetInvalid()
        {
                SetFlags(m_Flags | ERESULT_FLAG::INVALID);
        }
        void SetFlags(uint8_t flags)
        {
                m_Flags = flags;
        }
};

template <typename T>
class std::hash<Handle<T>>
{
    public:
        size_t operator()(const Handle<T> handle) const
        {
                return hash<uint32_t>()(handle.m_Id);
        }
};