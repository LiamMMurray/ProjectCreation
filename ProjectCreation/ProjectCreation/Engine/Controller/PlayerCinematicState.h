#pragma once

#include "PlayerControllerState.h"

class PlayerCinematicState : public IPlayerControllerState
{
    private:
        float alpha;

    public:
        // Inherited via IPlayerControllerState
        virtual void Enter() override;
        virtual void Update(float deltaTime) override;
        virtual void Exit() override;
};