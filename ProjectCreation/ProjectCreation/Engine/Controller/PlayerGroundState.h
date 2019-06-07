#pragma once
#include "PlayerControllerState.h"

class PlayerGroundState : public IPlayerControllerState
{
        // Inherited via IPlayerControllerState
        virtual void Enter() override;
        virtual void Update(float deltaTime) override;
        virtual void Exit() override;
};
