#pragma once

#include "..//..//ECS/ECSTypes.h"
#include "..//Gameplay/StateMachine/StateMachine.h"

class LevelStateMachine : public StateMachine
{
    public:
        void         Update(float deltaTime);
        void         Init();
        virtual void Shutdown() override;
};