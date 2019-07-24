#pragma once
#include "PlayerControllerState.h"

#include "../MathLibrary/MathLibrary.h"

class PlayerPuzzleState : public IPlayerControllerState
{
    private:
        float             goalDragMaxSpeed = 0.5f;
        DirectX::XMVECTOR dragVelocity;
        float             dragAcceleration   = 0.6f;
        float             dragDeacceleration = 0.9f;

        int m_CollectedPlanetCount = 0;

    public:
        // Inherited via IPlayerControllerState
        virtual void Enter() override;
        virtual void Update(float deltaTime) override;
        virtual void Exit() override;
        virtual ~PlayerPuzzleState() = default;
        inline void SetGoalDragSpeed(float val)
        {
                goalDragMaxSpeed = val;
        };

        inline float GetGoalDragSpeed() const
        {
                return goalDragMaxSpeed;
        }
};