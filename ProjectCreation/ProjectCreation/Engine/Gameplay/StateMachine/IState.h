#pragma once
#include <unordered_map>

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

		virtual ~IState() = default;
};