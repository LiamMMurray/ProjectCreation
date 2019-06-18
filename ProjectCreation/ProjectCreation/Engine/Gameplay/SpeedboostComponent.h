#pragma once
#include "../../ECS/Component.h"

#include <DirectXMath.h>
#include "LightOrbColors.h"

class SpeedboostComponent : public Component<SpeedboostComponent>
{
    public:
        float m_TargetRadius;
        float m_CurrentRadius;
        float m_CD    = 0.5f;
        float m_Timer = 0.0f;
        int   m_Color;
};