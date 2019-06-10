#pragma once
#include "../../ECS/Component.h"

#include "../GenericComponents/TransformComponent.h"

enum class E_GOAL_STATE
{
        Ready,
        Puzzle,
        Done,
        COUNT
};

class GoalComponent : public Component<GoalComponent>
{
    public:
        FTransform initialTransform;
        FTransform goalTransform;

        float        currAlpha              = 0.0f;
        float        targetAlpha            = 0.0f;
        float        transitionInitialSpeed = 0.1f;
        float        transitionFinalSpeed   = 1200.0f;
        E_GOAL_STATE goalState              = E_GOAL_STATE::Ready;
};