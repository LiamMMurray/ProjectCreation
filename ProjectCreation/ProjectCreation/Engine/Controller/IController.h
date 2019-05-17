#pragma once

#include "../../ECS/ECSTypes.h"

class IController
{
    private:

    protected:
        virtual void GatherInput()  = 0;
        virtual void ProcessInput() = 0;
        virtual void ApplyInput()   = 0;

        EntityHandle m_ControlledEntityHandle;

    public:
        virtual void OnUpdate(float deltaTime);
        virtual void Init(EntityHandle handle);
        inline void  SetControlledEntity(EntityHandle handle)
        {
                m_ControlledEntityHandle = handle;
        }

        inline EntityHandle SetControlledEntity()
        {
                return m_ControlledEntityHandle;
        }
};
