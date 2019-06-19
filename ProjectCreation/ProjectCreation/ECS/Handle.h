#pragma once
#include <stdint.h>
enum GHANDLEFLAG
{
        INACTIVE = 0b00000001,
        DESTROY  = 0b00000010
};
struct GHandleData
{
        uint32_t m_DeletionAccumulator : 24;
        uint32_t m_Flags : 8;

        bool IsValid()
        {
                return (m_Flags & GHANDLEFLAG::INACTIVE) == 0;
        }
        void SetInvalid()
        {
                SetFlags(m_Flags | GHANDLEFLAG::INACTIVE);
        }
		void SetValid()
		{
                SetFlags((m_Flags | GHANDLEFLAG::INACTIVE) ^ GHANDLEFLAG::INACTIVE);
		}
        void SetFlags(uint8_t flags)
        {
                m_Flags = flags;
        }
};
template <typename T>
struct GHandle
{
        GHandle() : m_Data(0)
        {}
        GHandle(uint32_t id, uint32_t version = 0U, uint8_t flags = 0U) :
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
                                GHandleData m_HandleData;
                                struct
                                {
                                        uint32_t m_DeletionAccumulator : 24;
                                        uint32_t m_Flags : 8;
                                };
                        };
                };
        };
        bool operator==(const GHandle<T>& other) const
        {
                return this->m_Data == other.m_Data;
        }
        bool IsValid()
        {
                return (m_Flags & GHANDLEFLAG::INACTIVE) == 0;
        }
        void SetInvalid()
        {
                SetFlags(m_Flags | GHANDLEFLAG::INACTIVE);
        }
        void SetFlags(uint8_t flags)
        {
                m_Flags = flags;
        }
};

template <typename T>
class std::hash<GHandle<T>>
{
    public:
        size_t operator()(const GHandle<T> handle) const
        {
                return hash<uint32_t>()(handle.m_Id);
        }
};