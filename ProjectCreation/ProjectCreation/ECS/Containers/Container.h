#pragma once
#include <stdint.h>
#include "FreeList.h"
#include "IContainer.h"

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
class Container : public IContainer<MainType>, public FreeList<MainType, SubType, MAX_ELEMENTS>
{
    private:
        typedef FreeList<MainType, SubType, MAX_ELEMENTS> Allocator;

    public:
        class iterator;

    public:
        // uint32_t  HandleId
        // MainType* raw data pointer
        std::vector<HandleData> m_HandleData;
        std::vector<MainType*>  m_ObjectData;

        // Handle<MainType> GetHandle(MainType* object)
        //{
        //        uint32_t i = 0;
        //        for (; i < m_ObjectData.size(); ++i)
        //        {
        //                if (m_ObjectData[i] == nullptr)
        //                {
        //                        m_ObjectData[i] = object;
        //                        m_HandleData[i] = HandleData();

        //                        Handle<MainType> out;
        //                        out.m_Id         = i;
        //                        out.m_HandleData = m_HandleData[i];
        //                        return out;
        //                }
        //                else if (m_ObjectData[i] == object)
        //                {
        //                        Handle<MainType> out;
        //                        out.m_Id         = i;
        //                        out.m_HandleData = m_HandleData[i];
        //                }
        //        }
        //        m_ObjectData.push_back(object);
        //        m_HandleData.push_back(HandleData());
        //        Handle<MainType> out;
        //        out.m_Id         = i;
        //        out.m_HandleData = m_HandleData[i];
        //        return out;
        //}
        //MainType* GetObject(Handle<MainType> handle)
        //{
        //        return m_ObjectData[handle.m_Id];
        //}
        size_t capacity()
        {
                return MAX_ELEMENTS;
        }
        iterator begin()
        {
                return iterator(*this);
        }
        iterator end()
        {
                return iterator(*this, m_HandleData.size());
        }

    public:
        void Initialize()
        {
                Allocator::Initialize();
        }
        Handle<MainType> GetHandle()
        {
                return Allocator::Allocate();
        }
		MainType* GetObject(Handle<MainType> handle)
		{
                return Allocator::operator[](handle);
		}
        void Free(Handle<MainType>& handle)
        {
                Allocator::Free(handle);
        };
        MainType* operator[](Handle<MainType> handle)
        {
                return static_cast<MainType*>(Allocator::operator[](handle));
        };
        void Shutdown()
        {
                Allocator::Shutdown();
        }
};

template <typename MainType, typename SubType, uint32_t MAX_ELEMENTS>
class Container<MainType, SubType, MAX_ELEMENTS>::iterator
{
    private:
        uint32_t                                    m_CurrentIndex;
        Container<MainType, SubType, MAX_ELEMENTS>& m_HandleManager;

    public:
        iterator(Container<MainType, SubType, MAX_ELEMENTS>& handleManager, uint32_t idx = 0U) :
            m_HandleManager(handleManager),
            m_CurrentIndex(idx){

            };
        iterator& operator++()
        {
                m_CurrentIndex++;
                while (m_CurrentIndex < m_HandleManager.size() && !m_HandleManager.m_ObjectData[m_CurrentIndex]->IsEnabled())
                {
                        m_CurrentIndex++;
                }
                return *this;
        };
        iterator operator++(int)
        {
                auto temp = *this;
                m_CurrentIndex++;
                while (m_CurrentIndex < m_HandleManager.capacity() &&
                       !m_HandleManager.m_ObjectData[m_CurrentIndex]->IsEnabled())
                {
                        m_CurrentIndex++;
                }
                return temp;
        };
        bool operator!=(const iterator& other) const
        {
                return this->m_CurrentIndex != other.m_CurrentIndex;
        };
        MainType& operator*()
        {
                return *m_HandleManager.m_ObjectData[m_CurrentIndex];
        };
        MainType* data()
        {
                return m_HandleManager.m_ObjectData[m_CurrentIndex];
        };
};