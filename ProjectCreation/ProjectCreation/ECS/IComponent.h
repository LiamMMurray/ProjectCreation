#pragma once
#include "ECSTypes.h"

class IComponent
{

    protected:
        bool            m_Enabled;
        Handle<IEntity>    m_Owner;
        Handle<IComponent> m_Handle;

    public:
        IComponent()
        {}

        virtual ~IComponent()
        {}


        Handle<IEntity> GetOwner();
        Handle<IComponent> GetHandle();
        void            Enable();
        void            Disable();
        void            SetActive(bool state);
        bool            isEnabled();
};