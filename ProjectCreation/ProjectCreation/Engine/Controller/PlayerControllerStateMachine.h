#pragma once

#include "..//..//ECS/ECSTypes.h"
#include "..//Gameplay/StateMachine/StateMachine.h"
#include "..//MathLibrary/Transform.h"
class PlayerController;
class TransformComponent;
class PlayerControllerStateMachine : public StateMachine
{
    protected:
    public:
        void         Update(float deltaTime, FTransform transform);
        void         Init(PlayerController* playerMovement);
        virtual void Shutdown() override;
};