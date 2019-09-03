#pragma once
#include <Component.h>

#include <TransformComponent.h>

enum class E_GOAL_STATE
{
        Spawning,
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

        ComponentHandle              collisionHandle;
        std::vector<ComponentHandle> goalRings;

        float        currAlpha              = 0.0f;
        float        targetAlpha            = 0.0f;
        float        transitionInitialSpeed = 300.0f;
        float        transitionFinalSpeed   = 800.0f;
        int          color;
        E_GOAL_STATE goalState = E_GOAL_STATE::Idle;
};