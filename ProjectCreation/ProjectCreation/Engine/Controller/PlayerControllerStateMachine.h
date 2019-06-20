#pragma once

#include "..//..//ECS/ECSTypes.h"
#include "..//Gameplay/StateMachine/StateMachine.h"

class PlayerController;
class TransformComponent;
class PlayerControllerStateMachine : public StateMachine
{
    protected:

    public:
        void Update(float deltaTime, TransformComponent* transformComp);
        void Init(PlayerController* playerMovement);
        virtual void Shutdown() override;
};