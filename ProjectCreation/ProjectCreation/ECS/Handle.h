#pragma once
#include <stdint.h>
enum HANDLEFLAG
{
        INACTIVE = 0b00000001,
        DESTROY  = 0b00000010
};
struct HandleData
{
        uint8_t  m_Flags : 8;
        uint32_t m_DeletionAccumulator : 24;
};
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
                        union
                        {
                                HandleData m_HandleData;
                                struct
                                {
                                        uint8_t  m_Flags : 8;
                                        uint32_t m_DeletionAccumulator : 24;
                                };
                        };
                };
        };
        bool operator==(const Handle<T>& other) const
        {
                return this->m_Data == other.m_Data;
        }
        bool IsValid()
        {
                return (m_Flags & HANDLEFLAG::INACTIVE) == 0;
        }
        void SetInvalid()
        {
                SetFlags(m_Flags | HANDLEFLAG::INACTIVE);
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