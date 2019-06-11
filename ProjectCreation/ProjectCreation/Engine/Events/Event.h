#pragma once
#include "IEvent.h"
#include "../../ECS/Util.h"
template <typename T>
class Event : public IEvent
{
    private:
        static const EventTypeId m_TypeId;

    public:
        Event();
        static const EventTypeId GetTypeId();
        const EventTypeId        GetStaticTypeId() const;
};
template <class T>
const EventTypeId Event<T>::m_TypeId = TypeIdCreator<IEvent>::GetUniqueTypeId<T>();

template <typename T>
inline Event<T>::Event()
{
        static_assert(std::is_base_of<Event<T>, T>::value, "Events must derive from Event<T>");
}

template <typename T>
inline const EventTypeId Event<T>::GetTypeId()
{
        static_assert(std::is_base_of<Event<T>, T>::value, "Events must derive from Event<T>");
        return m_TypeId;
}

template <typename T>
inline const EventTypeId Event<T>::GetStaticTypeId() const
{
        return m_TypeId;
}