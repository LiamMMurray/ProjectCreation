#pragma once
#include "../../ECS/Component.h"

#include "../GenericComponents/TransformComponent.h"

class GoalComponent : public Component<GoalComponent>
{
    public:
        FTransform initialTransform;
        FTransform goalTransform;

        float currAlpha            = 0.0f;
        float targetAlpha            = 0.0f;
        float transitionInitialSpeed = 0.1f;
        float transitionFinalSpeed = 1200.0f;
};