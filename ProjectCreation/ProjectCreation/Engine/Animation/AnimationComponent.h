#pragma once

#include "../../ECS/ECS.h"

#include "../../ResourceManager/AnimationClip.h"

struct AnimationComponent : Component<AnimationComponent>
{
        double                      m_Time;
        std::vector<ResourceHandle> m_Clips;
        std::vector<float>          m_Weights;
};
