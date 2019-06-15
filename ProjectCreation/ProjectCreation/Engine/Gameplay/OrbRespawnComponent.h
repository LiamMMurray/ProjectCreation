#pragma once
#include "../../ECS/Component.h"

class OrbRespawnComponent : public Component<OrbRespawnComponent>
{
    public:
        bool  m_WantsRespawn = false;
        float m_Lifetime;
};