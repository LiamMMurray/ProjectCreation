#pragma once

#include "..//Gameplay/StateMachine/IState.h"

class TransformComponent;
class PlayerController;
class IPlayerControllerState : public IState
{
        friend class PlayerControllerStateMachine;

    protected:
        TransformComponent* _cachedTransformComponent;
        PlayerController*   _playerController;
};