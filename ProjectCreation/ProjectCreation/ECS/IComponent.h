#pragma once
#include "ECSTypes.h"
class IComponent
{

    protected:
        bool            m_Enabled = true;
        EntityHandle    m_Owner;
        ComponentHandle m_Handle;

    public:
        IComponent()
        {}

        virtual ~IComponent()
        {}


        EntityHandle                  GetOwner();
        ComponentHandle               GetHandle();
        void                          Enable();
        void                          Disable();
        void                          SetActive(bool state);
        bool                          IsEnabled();
        virtual const ComponentTypeId GetStaticTypeId() const = 0;

        friend class ComponentManager;
};