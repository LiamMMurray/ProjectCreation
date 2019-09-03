#pragma once

#include "../..//Gameplay/StateMachine/IState.h"

#include <Transform.h>

class PlayerController;

class IPlayerControllerState : public IState
{
        friend class PlayerControllerStateMachine;

    protected:
        virtual void Enter()                 = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Exit()                  = 0;
        virtual ~IPlayerControllerState() = default;

        FTransform _cachedTransform;
        PlayerController*   _playerController;
};