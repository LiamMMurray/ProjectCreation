#pragma once
#include "../../Utility/MemoryLeakDetection.h"
#include "../Handle.h"

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
class FreeList
{
        union element
        {
                struct
                {
                        SubType e;
                };
                uint32_t next;
        };

    public:
        FreeList();

        ~FreeList();

    private:
        uint32_t m_free_start = 0;

        element*    m_pool           = 0;
        HandleData* m_pool_meta_data = 0;

    public:
        void Initialize();

        void Shutdown();


        Handle<MainType> Allocate();

        virtual void Free(Handle<MainType>& handle);

        SubType* operator[](Handle<MainType> handle);
};

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
inline FreeList<MainType, SubType, MAX_ELEMENTS>::FreeList()
{}

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
inline FreeList<MainType, SubType, MAX_ELEMENTS>::~FreeList()
{}

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
inline void FreeList<MainType, SubType, MAX_ELEMENTS>::Initialize()
{

        m_pool           = (element*)DBG_MALLOC(sizeof(element) * MAX_ELEMENTS);
        m_pool_meta_data = (HandleData*)DBG_MALLOC(sizeof(HandleData) * MAX_ELEMENTS);

        for (uint32_t i = 0; i < MAX_ELEMENTS; i++)
        {
                m_pool[i].next = i + 1;
        }
        for (uint32_t i = 0; i < MAX_ELEMENTS; i++)
        {
                m_pool_meta_data[i] = {};
        }
        m_pool[MAX_ELEMENTS - 1].next = -1;
}

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
inline void FreeList<MainType, SubType, MAX_ELEMENTS>::Shutdown()
{
        free(m_pool_meta_data);
        free(m_pool);
}

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
inline Handle<MainType> FreeList<MainType, SubType, MAX_ELEMENTS>::Allocate()
{
        Handle<MainType> out;
        out.m_Id                  = m_free_start;
        out.m_DeletionAccumulator = m_pool_meta_data[m_free_start].m_DeletionAccumulator;
        assert(out.m_Id != -1); // free list out of memory
        m_free_start = m_pool[out.m_Id].next;
        return out;
}

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
inline void FreeList<MainType, SubType, MAX_ELEMENTS>::Free(Handle<MainType>& handle)
{
        m_pool[handle.m_Id].next = m_free_start;
        m_pool_meta_data[handle.m_Id].m_DeletionAccumulator++;
        m_free_start = handle.m_Id;
}

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
inline SubType* FreeList<MainType, SubType, MAX_ELEMENTS>::operator[](Handle<MainType> handle)
{
        // accessing deleted object

        //assert(handle.m_DeletionAccumulator == m_pool_meta_data[handle.m_Id].m_DeletionAccumulator); 

        // if (handle.m_DeletionAccumulator != m_pool_meta_data[handle.m_Id].m_DeletionAccumulator) 
        //		return nullptr

        return &(m_pool[handle.m_Id].e);
}
