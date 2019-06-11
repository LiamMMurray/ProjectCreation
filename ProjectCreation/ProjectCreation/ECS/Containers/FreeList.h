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
        class iterator;
        uint32_t capacity()
        {
                return MAX_ELEMENTS;
        }
        iterator begin()
        {
                return iterator(*this);
        }
        iterator end()
        {
                return iterator(*this, MAX_ELEMENTS);
        }

    public:
        FreeList()
        {}

        ~FreeList()
        {}

    private:
        uint32_t m_free_start = 0;

        element*    m_pool           = 0;
        HandleData* m_pool_meta_data = 0;

    public:
        void Initialize()
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

        void Shutdown()
        {
                free(m_pool_meta_data);
                free(m_pool);
        }

        Handle<MainType> Allocate()
        {
                Handle<MainType> out;
                out.m_Id                  = m_free_start;
                out.m_DeletionAccumulator = m_pool_meta_data[m_free_start].m_DeletionAccumulator;
                assert(out.m_Id != -1); // free list out of memory
                m_free_start = m_pool[out.m_Id].next;
                m_pool_meta_data[out.m_Id].SetValid();
                return out;
        }

        virtual void Free(Handle<MainType>& handle)
        {
                m_pool[handle.m_Id].next = m_free_start;
                m_pool_meta_data[handle.m_Id].m_DeletionAccumulator++;
                m_pool_meta_data[handle.m_Id].SetInvalid();
                m_free_start = handle.m_Id;
        }

        SubType* operator[](Handle<MainType> handle)
        {
                // accessing deleted object

                // assert(handle.m_DeletionAccumulator == m_pool_meta_data[handle.m_Id].m_DeletionAccumulator);

                // if (handle.m_DeletionAccumulator != m_pool_meta_data[handle.m_Id].m_DeletionAccumulator)
                //		return nullptr

                return &(m_pool[handle.m_Id].e);
        }
};

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
class FreeList<MainType, SubType, MAX_ELEMENTS>::iterator
{
    private:
        uint32_t                                  m_CurrentIndex;
        FreeList<MainType, SubType, MAX_ELEMENTS> m_FreeList;

    public:
        iterator(FreeList<MainType, SubType, MAX_ELEMENTS>& cnt, uint32_t idx = 0U) : m_FreeList(cnt), m_CurrentIndex(idx)
        {}
        iterator& operator++()
        {
                m_CurrentIndex++;
                while (m_CurrentIndex < m_FreeList.size() && !m_FreeList.m_pool[m_CurrentIndex]->IsEnabled() &&
                       m_FreeList.m_pool_meta_data[m_CurrentIndex].IsInvalid())
                {
                        m_CurrentIndex++;
                }
                return *this;
        }
        iterator operator++(int)
        {
                auto temp = *this;
                m_CurrentIndex++;
                while (m_CurrentIndex < m_FreeList.capacity() && !m_FreeList.m_pool[m_CurrentIndex]->IsEnabled() &&
                       m_FreeList.m_pool_meta_data[m_CurrentIndex].IsInvalid())
                {
                        m_CurrentIndex++;
                }
                return temp;
        }
        bool operator!=(const iterator& other) const
        {
                return this->m_CurrentIndex != other.m_CurrentIndex;
        }
        MainType& operator*()
        {
                return *m_FreeList.m_ObjectData[m_CurrentIndex];
        }
        MainType* data()
        {
                return m_FreeList.m_ObjectData[m_CurrentIndex];
        }
};
