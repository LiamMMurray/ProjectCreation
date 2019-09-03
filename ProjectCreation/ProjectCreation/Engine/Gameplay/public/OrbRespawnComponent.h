#pragma once
#include <Component.h>

class OrbComponent : public Component<OrbComponent>
{
    public:
        bool  m_WantsDestroy = false;
        float m_TargetRadius;
        int   m_Color;
        float m_CurrentRadius;
};