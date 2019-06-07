#pragma once
#include <unordered_map>

enum class E_STATE_EVENT
{
        EASE_IN_START,
        EASE_IN_FINISH,
        EASE_OUT_START,
        EASE_OUT_FINISH,
        MOVEMENT_STOP,
        MOVEMENT_START,
        RHYTHM_MATCH,
        RHYTHM_FAIL,

};

class StateMachine;

class IState
{
        friend class StateMachine;

    protected:
        StateMachine* stateMachine = nullptr;

    public:
        virtual void Enter()                 = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Exit()                  = 0;
};