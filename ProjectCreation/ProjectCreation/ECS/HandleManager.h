#pragma once
#include <vector>
#include "Handle.h"

template <typename T>
class HandleManager
{
        // uint32_t  version number
        // T* raw data pointer
        std::vector<std::pair<uint32_t, T*>> m_HandleSpace;

    public:
        Handle<T> GetHandle(T* object)
        {
                uint32_t i = 0;
                for (; i < m_HandleSpace.size(); ++i)
                {
                        if (m_HandleSpace[i].second == nullptr)
                        {
                                m_HandleSpace[i].second = object;
                                m_HandleSpace[i].first  = m_HandleSpace[i].first;
                                return Handle<T>(i, m_HandleSpace[i].first);
                        }
                        else if (m_HandleSpace[i].second == object)
                                return Handle<T>(i, m_HandleSpace[i].first);
                }
                m_HandleSpace.push_back(std::make_pair(0, object));
                return Handle<T>(i, m_HandleSpace[i].first);
        }
		T* GetObject(Handle<T> handle)
		{
                return m_HandleSpace[handle.m_Id];
		}
};