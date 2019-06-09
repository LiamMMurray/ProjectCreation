#pragma once
#include "IEventDelegate.h"
#include "../../ECS/Util.h"
class IEvent;
template <typename T, typename EventType>
class EventDelegate : public IEventDelegate
{
    private:
        static const EventDelegateTypeId m_TypeId;
        void (*m_DelegateFunction)(EventType* e);

    public:
        EventDelegate(void (*delegateFunction)(EventType*));
        static const EventDelegateTypeId GetTypeId();
        const EventDelegateTypeId        GetStaticTypeId() const;
        void                             Invoke(IEvent* e);
};
template <class T, typename EventType>
const EventDelegateTypeId EventDelegate<T, EventType>::m_TypeId = TypeUtility<IEventDelegate>::GetUniqueTypeId<T>();

template <typename T, typename EventType>
inline EventDelegate<T, EventType>::EventDelegate(void (*delegateFunction)(EventType*))
{
        m_DelegateFunction = delegateFunction;
        static_assert(std::is_base_of<EventDelegate<T, EventType>, T>::value, "this class must follow CRTP");
}

template <typename T, typename EventType>
inline const EventDelegateTypeId EventDelegate<T, EventType>::GetTypeId()
{
        static_assert(std::is_base_of<EventDelegate<T, EventType>, T>::value, "this class must follow CRTP");
        return m_TypeId;
}

template <typename T, typename EventType>
inline const EventDelegateTypeId EventDelegate<T, EventType>::GetStaticTypeId() const
{
        return m_TypeId;
}

template <typename T, typename EventType>
inline void EventDelegate<T, EventType>::Invoke(IEvent* e)
{
        m_DelegateFunction(static_cast<EventType*>(e));
}
