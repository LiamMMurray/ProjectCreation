#pragma once
#include "Event.h"
#include "EventDelegate.h"
#include <vector>
template <typename DelegateType>
class EventManager
{
    private:
        std::vector<DelegateType> m_Delegates;
    public:
        void AddEventListener(DelegateType _pDelegate)
		{
                m_Delegates.push_back(_pDelegate);
		}
        void RemoveEventListener() 
        {}
};