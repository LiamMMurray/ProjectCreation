#pragma once
#include <DirectXMath.h>
#include "IController.h"
#include "../ECS/ECSTypes.h"

class PlayerMovement : public IController
{
    private:
        void GatherInput() override;
        void ProcessInput() override;
        void ApplyInput() override;

		void ModifySpeed();

    public:
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

		Handle<IComponent> m_handle;

		int SpeedModifier;

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
        DirectX::XMVECTOR MoveVector;
};