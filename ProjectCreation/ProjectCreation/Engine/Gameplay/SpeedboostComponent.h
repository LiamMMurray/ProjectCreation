#pragma once
#include "../../ECS/Component.h"

#include <DirectXMath.h>
#include "LightOrbColors.h"

class SpeedboostComponent : public Component<SpeedboostComponent>
{
    public:
        bool  m_WantsRespawn = false;
        bool  m_CanRespawn   = false;
        float m_TargetRadius;
        float m_CurrentRadius;
        float m_Lifetime;
        float m_CD    = 0.5f;
        float m_Timer = 0.0f;
        int   m_Color;
};