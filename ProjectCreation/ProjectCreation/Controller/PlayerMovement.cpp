#include "PlayerMovement.h"
#include "../CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"
#include "../ECS/Entity.h"
#include "../Components/PhysicsComponent.h"

// v Testing only delete when done v
#include <iostream>
// ^ Testing only delete when done ^

void PlayerMovement::GatherInput()
{
		PastDirection = requestedDirection;

        //requestedDirection = MoveDirections::NO_DIRECTION;

        MoveVector = ZeroVector;
        // Check Forward speed
        ModifySpeed();

        // Backward
        if (GCoreInput::GetKeyState(KeyCode::S) == KeyState::Down)
        {
                requestedDirection = MoveDirections::BACKWARD;
        }
        // Left
        else if (GCoreInput::GetKeyState(KeyCode::A) == KeyState::Down)
        {
                requestedDirection = MoveDirections::LEFT;
        }
        // Right
        else if (GCoreInput::GetKeyState(KeyCode::D) == KeyState::Down)
        {
                requestedDirection = MoveDirections::RIGHT;
        }
}

void PlayerMovement::ProcessInput()
{

        // Forward
        if (requestedDirection == MoveDirections::FORWARD)
        {
                MoveVector = DirectX::XMVectorAdd(MoveVector, forwardVector);
        }
        // Backward
        else if (requestedDirection == MoveDirections::BACKWARD)
        {
                MoveVector = DirectX::XMVectorAdd(MoveVector, backwardVector);
        }
        // Left
        else if (requestedDirection == MoveDirections::LEFT)
        {
                MoveVector = DirectX::XMVectorAdd(MoveVector, leftVector);
        }
        // Right
        else if (requestedDirection == MoveDirections::RIGHT)
        {
                MoveVector = DirectX::XMVectorAdd(MoveVector, rightVector);
        }

}

void PlayerMovement::ApplyInput()
{
        PhysicsComponent physicsComponent;
		
		physicsComponent.AddForce(MoveVector);
}

void PlayerMovement::ModifySpeed()
{
        SpeedModifier = 0;
        if (GCoreInput::GetKeyState(KeyCode::W) == KeyState::Down)
        {
                requestedDirection = MoveDirections::FORWARD;
                SpeedModifier += 1;
        }

        if (GCoreInput::GetKeyState(KeyCode::Q) == KeyState::Down)
        {
                requestedDirection = MoveDirections::FORWARD;
                SpeedModifier += 1;
        }

        if (GCoreInput::GetKeyState(KeyCode::E) == KeyState::Down)
        {
                requestedDirection = MoveDirections::FORWARD;
                SpeedModifier += 1;
        }
}