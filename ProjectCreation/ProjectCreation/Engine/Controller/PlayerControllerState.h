#pragma once

#include "..//Gameplay/StateMachine/IState.h"

class TransformComponent;
class PlayerController;


class IPlayerControllerState : public IState
{
        friend class PlayerControllerStateMachine;

    protected:
        virtual void Enter()                 = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Exit()                  = 0;

        TransformComponent* _cachedTransformComponent;
        PlayerController*   _playerController;
};