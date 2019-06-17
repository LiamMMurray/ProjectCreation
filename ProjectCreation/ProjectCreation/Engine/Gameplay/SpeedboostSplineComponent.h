#pragma once

#include "../../ECS/Component.h"

class SplineElementComponent : public Component<SplineElementComponent>
{
    public:
        ComponentHandle m_NextOrb;
        ComponentHandle m_PreviousOrb;
};