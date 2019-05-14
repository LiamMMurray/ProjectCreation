#include "PlayerMovement.h"
#include "../Components/PhysicsComponent.h"
#include "../CoreInput/CoreInput.h"
#include "../ECS/Entity.h"
#include "../Engine/GEngine.h"

// v Testing only delete when done v
#include <iostream>
// ^ Testing only delete when done ^

void PlayerMovement::GatherInput()
{
        PastDirection = requestedDirection;

        // requestedDirection = MoveDirections::NO_DIRECTION;

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
        physicsComponent.SetMass(1.0f);
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

PhysicsComponent   PlayerMovement::physicsComponent;
TransformComponent PlayerMovement::transformComponent;