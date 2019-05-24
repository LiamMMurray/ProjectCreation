#pragma once

#include <DirectXMath.h>
#include "../../ECS/ECSTypes.h"

class IController
{
    protected:
        virtual void GatherInput()  = 0;
        virtual void ProcessInput() = 0;
        virtual void ApplyInput()   = 0;

        EntityHandle m_ControlledEntityHandle;

		float cacheTime;

    public:
        virtual void OnUpdate(float deltaTime);
        virtual void Init(EntityHandle handle);
        inline void  SetControlledEntity(EntityHandle handle)
        {
                m_ControlledEntityHandle = handle;
        }

        inline EntityHandle GetControlledEntity()
        {
                return m_ControlledEntityHandle;
        }
};
