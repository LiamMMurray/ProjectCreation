#pragma once
#include <ECSTypes.h>
class IEventDelegate;
class IEvent;
typedef ECSTypeId<IEventDelegate> EventDelegateTypeId;
class IEventDelegate
{
    public:
        IEventDelegate()
        {}

        virtual ~IEventDelegate()
        {}

        virtual const EventDelegateTypeId GetStaticTypeId() const = 0;
        virtual void                      Invoke(IEvent* e)       = 0;
};