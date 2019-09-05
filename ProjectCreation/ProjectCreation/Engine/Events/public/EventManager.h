#pragma once
#include <vector>
#include <Event.h>
#include <EventDelegate.h>
template <typename DelegateType, typename EventType>
class EventManager
{
    private:
        std::vector<DelegateType> m_Delegates;

    public:
        void AddEventListener(DelegateType _pDelegate)
        {
                m_Delegates.push_back(_pDelegate);
        }
		void AddEventListener(void (*delegateFunc)(EventType*))
		{
                AddEventListener(DelegateType(delegateFunc));
		}
        void RemoveEventListener()
        {}
        void Invoke(EventType* e)
        {
                for (DelegateType& d : m_Delegates)
                {
                        d.Invoke(e);
                }
        }
};