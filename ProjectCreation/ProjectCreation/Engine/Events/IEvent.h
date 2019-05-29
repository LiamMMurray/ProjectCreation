#pragma once
#include "../ECS/ECSTypes.h"
class IEvent;
typedef ECSTypeId<IEvent> EventTypeId;
class IEvent
{
    public:
        IEvent()
        {}

        virtual ~IEvent()
        {}

        virtual const EventTypeId GetStaticTypeId() const = 0;
};