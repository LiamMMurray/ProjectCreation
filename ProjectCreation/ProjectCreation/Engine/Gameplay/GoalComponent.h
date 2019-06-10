#pragma once
#include "../../ECS/Component.h"

#include "../GenericComponents/TransformComponent.h"

enum class E_GOAL_STATE
{
        Idle,
        InitialTransition,
        Puzzle,
		Done,
        COUNT
};

class GoalComponent : public Component<GoalComponent>
{
    public:
        FTransform initialTransform;
        FTransform goalTransform;

        ComponentHandle collisionHandle;

        float        currAlpha              = 0.0f;
        float        targetAlpha            = 0.0f;
        float        transitionInitialSpeed = 300.0f;
        float        transitionFinalSpeed   = 800.0f;
        E_GOAL_STATE goalState              = E_GOAL_STATE::Idle;
};