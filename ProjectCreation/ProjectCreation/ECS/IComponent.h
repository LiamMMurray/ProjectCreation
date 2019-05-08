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

		// required to implement functions
        virtual TypeId          GetTypeID() = 0;
		// implement functions
        EntityHandle    GetOwner();
        ComponentHandle GetHandle();
        void            Enable();
        void            Disable();
        void            SetActive(bool state);
        bool            isEnabled();
};