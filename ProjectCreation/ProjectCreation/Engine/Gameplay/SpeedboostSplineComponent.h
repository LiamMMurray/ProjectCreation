#pragma once

#include "../../ECS/Component.h"

class SpeedboostSplineComponent : public Component<SpeedboostSplineComponent>
{
    public:
        ComponentHandle m_NextOrb;
        ComponentHandle m_PreviousOrb;
};