#include "PlayerGroundState.h"
#include "..//GEngine.h"
#include "..//GenericComponents/TransformComponent.h"
#include "..//MathLibrary/MathLibrary.h"
#include "../CoreInput/CoreInput.h"
#include "PlayerControllerStateMachine.h"
#include "PlayerMovement.h"

using namespace DirectX;

void PlayerGroundState::Enter()
{
        auto playerTransformComponent =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(_playerController->GetControlledEntity());

        _playerController->SetEulerAngles(playerTransformComponent->transform.rotation.ToEulerAngles());
}

void PlayerGroundState::Update(float deltaTime)
{
        // Get Delta Time
        float totalTime = (float)GEngine::Get()->GetTotalTime();

        // Get the Speed from the gathered input
        XMVECTOR currentInput = _playerController->GetCurrentInput();
        float    currSpeed    = XMVectorGetX(XMVector3Length(currentInput));

        // Get the Forward from the gathered input
        float currForward = XMVectorGetZ(currentInput);

        // Normalize the gathered input to determine the desired direction
        XMVECTOR desiredDir = XMVector3Normalize(currentInput);

        // Determine the max speed the object can move
        float maxSpeed = MathLibrary::lerp(_playerController->GetMinMaxSpeed(),
                                           _playerController->GetMaxMaxSpeed(),
                                           currForward - _playerController->GetMinMaxSpeed());

        // Check if the currSpeed is faster than the maxSpeed
        if (fabs(currSpeed) > fabs(maxSpeed))
        {
                // Clamp the currSpeed to the maxSpeed
                currSpeed = MathLibrary::clamp(currSpeed, -maxSpeed, maxSpeed);
        }

        // Calculate desiredVelocity by multiplying the currSpeed by the direction we want to go
        XMVECTOR desiredVelocity = currSpeed * desiredDir;
        XMVECTOR currentVelocity = _playerController->GetCurrentVelocity();
        // Determine if we should speed up or slow down
        float accel = (XMVectorGetX(XMVector3Length(desiredVelocity)) >= XMVectorGetX(XMVector3Length(currentVelocity))) ?
                          _playerController->GetAcceleration() :
                          _playerController->GetDeacceleration();

        // Calculate distance from our current velocity to our desired velocity
        float dist = MathLibrary::CalulateDistance(currentVelocity, desiredVelocity);

        // Calculate change based on the type of acceleration, the change in time, and the calculated distance
        float delta = std::min(accel * deltaTime, dist);

        // Normalize the difference of the desired velocity and the current velocity
        XMVECTOR deltaVec = XMVector3Normalize(desiredVelocity - currentVelocity);

        // Calculate current velocity based on itself, the deltaVector, and delta
        currentVelocity = currentVelocity + deltaVec * delta;

        XMVECTOR preBoostVelocity = XMVectorZero();

        currentVelocity = XMVectorLerp(currentVelocity, preBoostVelocity, MathLibrary::clamp(deltaTime * 0.001f, 0.0f, 1.0f));

        XMFLOAT3 eulerAngles = _playerController->GetEulerAngles();

        /*if (totalTime >= 2.0f && totalTime <= 5.0f)
        {
                eulerAngles.x = MathLibrary::lerp(eulerAngles.x, 0.0f, MathLibrary::clamp(deltaTime * 0.5f, 0.0f, 1.0f));
        }*/

        float           angularSpeed = XMConvertToRadians(2.0f) * deltaTime;
        constexpr float pitchLimit   = XMConvertToRadians(90.0f);
        constexpr float rollLimit    = 20.0f;

        eulerAngles.x += GCoreInput::GetMouseY() * angularSpeed;
        eulerAngles.y += GCoreInput::GetMouseX() * angularSpeed;
        eulerAngles.z += GCoreInput::GetMouseX() * angularSpeed;

        eulerAngles.x = MathLibrary::clamp(eulerAngles.x, -pitchLimit, pitchLimit);

        // Convert to degrees due to precision errors using small radian values
        float rollDegrees = XMConvertToDegrees(eulerAngles.z);
        rollDegrees       = MathLibrary::clamp(rollDegrees, -rollLimit, rollLimit);
        rollDegrees       = MathLibrary::lerp(rollDegrees, 0.0f, MathLibrary::clamp(deltaTime * rollLimit, 0.0f, 1.0f));
        eulerAngles.z     = XMConvertToRadians(rollDegrees);
        _cachedTransformComponent->transform.rotation = FQuaternion::FromEulerAngles(eulerAngles);


        // Calculate offset
        XMVECTOR offset = XMVector3Rotate(currentVelocity * deltaTime, _cachedTransformComponent->transform.rotation.data);
        offset          = XMVector3Normalize(XMVectorSetY(offset, 0.0f)) * XMVectorGetX(XMVector3Length(offset));
        _cachedTransformComponent->transform.translation += offset;

        _playerController->SetCurrentVelocity(currentVelocity);
        _playerController->SetEulerAngles(eulerAngles);
}

void PlayerGroundState::Exit()
{
        _playerController->SetCurrentVelocity(XMVectorZero());
}
