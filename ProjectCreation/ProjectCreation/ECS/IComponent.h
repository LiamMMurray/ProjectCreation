#pragma once
#include "ECSTypes.h"
#include "Handle.h"

class IComponent
{

    protected:
        bool            m_Enabled;
        EntityHandle    m_Owner;
        ComponentHandle m_Handle;

    public:
        IComponent()
        {}

        virtual ~IComponent()
        {}


        EntityHandle    GetOwner();
        ComponentHandle GetHandle();
        void            Enable();
        void            Disable();
        void            SetActive(bool state);
        bool            isEnabled();
};