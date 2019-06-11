#pragma once
#include <unordered_map>
#include <vector>
#include "Handle.h"
#include "Containers/Container.h"

template <typename T>
class HandleManager
{
    public:
        class iterator;

    public:
        // uint32_t  HandleId
        // T* raw data pointer
        std::vector<HandleData> m_HandleData;
        std::vector<T*>         m_ObjectData;

        Handle<T> GetHandle(T* object)
        {
                uint32_t i = 0;
                for (; i < m_ObjectData.size(); ++i)
                {
                        if (m_ObjectData[i] == nullptr)
                        {
                                m_ObjectData[i] = object;
                                m_HandleData[i] = HandleData();

                                Handle<T> out;
                                out.m_Id         = i;
                                out.m_HandleData = m_HandleData[i];
                                return out;
                        }
                        else if (m_ObjectData[i] == object)
                        {
                                Handle<T> out;
                                out.m_Id         = i;
                                out.m_HandleData = m_HandleData[i];
                        }
                }
                m_ObjectData.push_back(object);
                m_HandleData.push_back(HandleData());
                Handle<T> out;
                out.m_Id         = i;
                out.m_HandleData = m_HandleData[i];
                return out;
        }
        T* GetObject(Handle<T> handle)
        {
                return m_ObjectData[handle.m_Id];
        }
        size_t size()
        {
                return m_HandleData.size();
        }
        iterator begin()
        {
                return iterator(*this);
        }
        iterator end()
        {
                return iterator(*this, m_HandleData.size());
        }
        T& operator[](uint32_t idx)
        {
                return *m_ObjectData[idx];
        }
};

template <typename T>
class HandleManager<T>::iterator
{
    private:
        uint32_t          m_CurrentIndex;
        HandleManager<T>& m_HandleManager;

    public:
        iterator(HandleManager<T>& handleManager, uint32_t idx = 0U);
        iterator& operator++();
        iterator  operator++(int);
        bool      operator!=(const iterator& other) const;
        T&        operator*();
        T*        data();
};

template <typename T>
HandleManager<T>::iterator::iterator(HandleManager<T>& handleManager, uint32_t idx) :
    m_HandleManager(handleManager),
    m_CurrentIndex(idx){

    };

template <typename T>
typename bool HandleManager<T>::iterator::operator!=(const iterator& other) const
{
        return this->m_CurrentIndex != other.m_CurrentIndex;
};

template <typename T>
typename T* HandleManager<T>::iterator::data()
{
        return m_HandleManager.m_ObjectData[m_CurrentIndex];
};

template <typename T>
typename HandleManager<T>::iterator& HandleManager<T>::iterator::operator++()
{
        m_CurrentIndex++;
        while (m_CurrentIndex < m_HandleManager.size() && !m_HandleManager.m_ObjectData[m_CurrentIndex]->IsEnabled())
        {
                m_CurrentIndex++;
        }
        return *this;
};

template <typename T>
typename HandleManager<T>::iterator HandleManager<T>::iterator::operator++(int)
{
        auto temp = *this;
        m_CurrentIndex++;
        while (m_CurrentIndex < m_HandleManager.size() && !m_HandleManager.m_ObjectData[m_CurrentIndex]->IsEnabled())
        {
                m_CurrentIndex++;
        }
        return temp;
};

template <typename T>
typename T& HandleManager<T>::iterator::operator*()
{
        return *m_HandleManager.m_ObjectData[m_CurrentIndex];
};