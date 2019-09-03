#pragma once
#include <PlayerControllerState.h>

#include <MathLibrary.h>
// Audio Includes
#include <Interface/G_Audio/GMusic.h>
#include <Interface/G_Audio/GSound.h>
#include <AudioManager.h>

class PlayerPuzzleState : public IPlayerControllerState
{
    private:
        float             goalDragMaxSpeed = 0.5f;
        DirectX::XMVECTOR dragVelocity;
        float             dragAcceleration   = 0.6f;
        float             dragDeacceleration = 0.9f;


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