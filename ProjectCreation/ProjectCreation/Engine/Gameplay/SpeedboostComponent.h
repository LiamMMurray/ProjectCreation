#pragma once
#include "../../ECS/Component.h"

#include <DirectXMath.h>
#include "LightOrbColors.h"

class SpeedboostComponent : public Component<SpeedboostComponent>
{
    public:
        float m_TargetRadius;
        float m_CurrentRadius;
        int   m_Color;
};