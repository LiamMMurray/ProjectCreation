#pragma once
#include "../../ECS/Component.h"

#include "LightOrbColors.h"

class SpeedboostComponent : public Component<SpeedboostComponent>
{
    public:
        bool  m_WantsRespawn = false;
        float m_TargetRadius;
        float m_CurrentRadius;
        float m_Lifetime;
        E_LIGHT_ORBS m_Color;
};