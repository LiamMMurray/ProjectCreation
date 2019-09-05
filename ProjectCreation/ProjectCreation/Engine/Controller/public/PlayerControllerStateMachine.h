#pragma once

#include <ECSTypes.h>
#include <StateMachine.h>
#include <Transform.h>
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