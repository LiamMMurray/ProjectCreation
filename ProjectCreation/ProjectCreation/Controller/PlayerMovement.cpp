#include "PlayerMovement.h"
#include "../CoreInput/CoreInput.h"
#include "../Engine/GEngine.h"

void PlayerMovement::GatherInput()
{
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
       // auto transformComponent = GEngine::Get()->GetComponentManager()->GetComponent<Transform>(handle);

        // Code will not work until Transform Component is made
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
                SpeedModifier += 1;
        }

        if (GCoreInput::GetKeyState(KeyCode::E) == KeyState::Down)
        {
                SpeedModifier += 1;
        }
}
