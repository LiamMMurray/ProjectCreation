#pragma once
#include "../../ECS/ECSTypes.h"
#include "IController.h"

#include "../Physics/PhysicsComponent.h"

class PlayerController : public IController
{
    private:
        void GatherInput() override;
        void ProcessInput() override;
        void ApplyInput() override;

        void PauseInput() override;
		
		DirectX::XMVECTOR m_CurrentPosition;

		struct FDebugLine
		{
                DirectX::XMVECTOR start;
                DirectX::XMVECTOR end;
		};

    public:
        void InactiveUpdate(float deltaTime) override;
        PlayerController();

        enum MoveDirections
        {
                NO_DIRECTION = 0,
                FORWARD,
                BACKWARD,
                LEFT,
                RIGHT
        };

        enum PlayerStates
        {
                ON_GROUND = 0,
                ON_AIR,
                ON_WATER
        };

        // Requested direction is set when we process the player's input
        MoveDirections requestedDirection;

        // Forward is the players local positive Z
        DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

        // Backward is the players local Negative Z
        DirectX::XMVECTOR backwardVector = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);

        // Right is the players local positive X
        DirectX::XMVECTOR rightVector = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

        // Left is the players local Negative X
        DirectX::XMVECTOR leftVector = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);

        // ZeroVector will be the default vector and will be used to reset movement
        DirectX::XMVECTOR ZeroVector = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        // MoveVector will be used to apply the movement to the player
        DirectX::XMVECTOR MoveVector = ZeroVector;

        // PastDirection is used for debugging, delete when done
        MoveDirections PastDirection;

		float minMaxSpeed = 1.0f;
        float maxMaxSpeed = 3.0f;

		float acceleration = 2.0;
        float deacceleration = 1.5f;

		int32_t m_MouseXDelta;
        int32_t m_MouseYDelta;

		DirectX::XMVECTOR m_CurrentInput;

        DirectX::XMVECTOR m_CurrentVelocity;

};