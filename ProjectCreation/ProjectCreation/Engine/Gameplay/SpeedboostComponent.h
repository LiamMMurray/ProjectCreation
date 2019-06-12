#pragma once
#include "../../ECS/Component.h"

#include <DirectXMath.h>
#include "LightOrbColors.h"

struct FSpeedboostSpawnSettings
{
        float             lifetime;
        int               color;
        DirectX::XMVECTOR pos;
        bool              randomLifetime = true;

        FSpeedboostSpawnSettings() = default;
        FSpeedboostSpawnSettings(const DirectX::XMVECTOR& p, int c, bool r, float l = -1.0f) :
            lifetime(l),
            color(c),
            pos(p),
            randomLifetime(r){};
};

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