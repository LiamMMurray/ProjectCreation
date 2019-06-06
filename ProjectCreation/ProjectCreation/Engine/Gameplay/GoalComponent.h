#pragma once
#include "../../ECS/Component.h"

#include "../GenericComponents/TransformComponent.h"

class GoalComponent : public Component<GoalComponent>
{
    public:
        FTransform initialTransform;
        FTransform goalTransform;

        float currAlpha       = 0.0f;
        float targetAlpha     = 0.0f;
        float transitionSpeed = 0.0f;
};