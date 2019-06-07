#pragma once

#include <DirectXMath.h>
#include "../../ECS/ECSTypes.h"

class IController
{
        bool m_Enabled = true;

    protected:
        virtual void GatherInput()  = 0;
        virtual void ProcessInput() = 0;
        virtual void ApplyInput()   = 0;


        EntityHandle m_ControlledEntityHandle;

        float cacheTime;


    public:
        inline void SetEnabled(bool val)
        {
                m_Enabled = val;
        }

        inline bool IsEnabled()
        {
                return m_Enabled;
        }

        virtual void OnUpdate(float deltaTime);

        virtual void Init(EntityHandle handle);

        inline void SetControlledEntity(EntityHandle handle)
        {
                m_ControlledEntityHandle = handle;
        }

        inline EntityHandle GetControlledEntity()
        {
                return m_ControlledEntityHandle;
        }
};
