#include "PlayerMovement.h"
#include "../../ECS/Entity.h"
#include "../CoreInput/CoreInput.h"
#include "../GEngine.h"

#include "../MathLibrary/MathLibrary.h"
#include "../MathLibrary/Quaternion.h"

#include "..//Gameplay/GoalComponent.h"
#include "../CollisionLibary/CollisionLibary.h"
#include "../CollisionLibary/CollisionResult.h"
#include "../GenericComponents/TransformComponent.h"

#include "../../Rendering/DebugRender/debug_renderer.h"
// v Testing only delete when done v
#include <iostream>
// ^ Testing only delete when done ^
using namespace DirectX;
using namespace Shapes;
using namespace Collision;
using namespace debug_renderer;
void PlayerController::GatherInput()
{
        m_TotalTime += cacheTime;

        m_CurrentInput = XMVectorZero();
        if (IsEnabled() && m_TotalTime >= 5.0f)
        {
                // requestedDirection = MoveDirections::NO_DIRECTION;
                XMFLOAT4 tempDir = {0.0f, 0.0f, 0.0f, 0.0f};

                // Check Forward speed
                if (GCoreInput::GetKeyState(KeyCode::W) == KeyState::Down)
                {
                        tempDir.z += 1.0f;
                }

                if (GCoreInput::GetKeyState(KeyCode::Q) == KeyState::Down)
                {
                        tempDir.z += 1.0f;
                }

                if (GCoreInput::GetKeyState(KeyCode::E) == KeyState::Down)
                {
                        tempDir.z += 1.0f;
                }
                // Backward
                if (GCoreInput::GetKeyState(KeyCode::S) == KeyState::Down)
                {
                        tempDir.z -= 1.0f;
                }
                // Left
                if (GCoreInput::GetKeyState(KeyCode::A) == KeyState::Down)
                {
                        tempDir.x -= 1.0f;
                }
                // Right
                if (GCoreInput::GetKeyState(KeyCode::D) == KeyState::Down)
                {
                        tempDir.x += 1.0f;
                }
                // Rise
                if (GCoreInput::GetKeyState(KeyCode::Space) == KeyState::Down)
                {
                        tempDir.y += 1.0f;
                }
                // Fall
                if (GCoreInput::GetKeyState(KeyCode::Control) == KeyState::Down)
                {
                        tempDir.y -= 1.0f;
                }

                m_MouseXDelta = GCoreInput::GetMouseX();
                m_MouseYDelta = GCoreInput::GetMouseY();

                m_CurrentInput = XMLoadFloat4(&tempDir);
        }
}

void PlayerController::ProcessInput()
{
        _cachedControlledTransformComponent =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_ControlledEntityHandle);
}

void PlayerController::ApplyInput()
{
        switch (m_CurrentPlayerState)
        {
                case EPlayerState::ON_GROUND:
                {
                        UpdateOnGround();
                        break;
                }
                case EPlayerState::ON_PUZZLE:
                {
                        UpdateOnPuzzle();
                        break;
                }
                default:
                {
                        assert(false && "INVALID CURRENT PLAYER STATE");
                        break;
                }
        }

        m_CurrentPosition = _cachedControlledTransformComponent->transform.translation;

        FSphere fSpherePlayer;
        fSpherePlayer.center = m_CurrentPosition;
        fSpherePlayer.radius = 0.25f;

        debug_renderer::AddSphere(fSpherePlayer, 36, XMMatrixIdentity());
}

void PlayerController::UpdateOnGround()
{
        // Get Delta Time
        float deltaTime = cacheTime; // GEngine::Get()->GetDeltaTime();

        // Get the Speed from the gathered input
        float currSpeed = XMVectorGetX(XMVector3Length(m_CurrentInput));

        // Get the Forward from the gathered input
        float currForward = XMVectorGetZ(m_CurrentInput);

        // Normalize the gathered input to determine the desired direction
        XMVECTOR desiredDir = XMVector3Normalize(m_CurrentInput);

        // Determine the max speed the object can move
        float maxSpeed = MathLibrary::lerp(minMaxSpeed, maxMaxSpeed, currForward - minMaxSpeed);

        // Check if the currSpeed is faster than the maxSpeed
        if (fabs(currSpeed) > fabs(maxSpeed))
        {
                // Clamp the currSpeed to the maxSpeed
                currSpeed = MathLibrary::clamp(currSpeed, -maxSpeed, maxSpeed);
        }

        // Calculate desiredVelocity by multiplying the currSpeed by the direction we want to go
        XMVECTOR desiredVelocity = currSpeed * desiredDir;

        // Determine if we should speed up or slow down
        float accel = (XMVectorGetX(XMVector3Length(desiredVelocity)) >= XMVectorGetX(XMVector3Length(m_CurrentVelocity))) ?
                          acceleration :
                          deacceleration;

        // Calculate distance from our current velocity to our desired velocity
        float dist = MathLibrary::CalulateDistance(m_CurrentVelocity, desiredVelocity);

        // Calculate change based on the type of acceleration, the change in time, and the calculated distance
        float delta = std::min(accel * deltaTime, dist);

        // Normalize the difference of the desired velocity and the current velocity
        XMVECTOR deltaVec = XMVector3Normalize(desiredVelocity - m_CurrentVelocity);

        // Calculate current velocity based on itself, the deltaVector, and delta
        m_CurrentVelocity = m_CurrentVelocity + deltaVec * delta;

        XMVECTOR preBoostVelocity = XMVectorZero();

        m_CurrentVelocity =
            XMVectorLerp(m_CurrentVelocity, preBoostVelocity, MathLibrary::clamp(deltaTime * 0.25f, 0.0f, 1.0f));

        if (m_TotalTime >= 2.0f && m_TotalTime <= 5.0f)
        {
                m_EulerAngles.x = MathLibrary::lerp(m_EulerAngles.x, 0.0f, MathLibrary::clamp(deltaTime * 0.5f, 0.0f, 1.0f));
        }

        float           angularSpeed = XMConvertToRadians(2.0f) * deltaTime;
        constexpr float pitchLimit   = XMConvertToRadians(90.0f);
        constexpr float rollLimit    = 20.0f;

        m_EulerAngles.x += m_MouseYDelta * angularSpeed;
        m_EulerAngles.y += m_MouseXDelta * angularSpeed;
        m_EulerAngles.z += m_MouseXDelta * angularSpeed;

        m_EulerAngles.x = MathLibrary::clamp(m_EulerAngles.x, -pitchLimit, pitchLimit);

        // Convert to degrees due to precision errors using small radian values
        float rollDegrees = XMConvertToDegrees(m_EulerAngles.z);
        rollDegrees       = MathLibrary::clamp(rollDegrees, -rollLimit, rollLimit);
        rollDegrees       = MathLibrary::lerp(rollDegrees, 0.0f, MathLibrary::clamp(deltaTime * rollLimit, 0.0f, 1.0f));
        m_EulerAngles.z   = XMConvertToRadians(rollDegrees);
        _cachedControlledTransformComponent->transform.rotation = FQuaternion::FromEulerAngles(m_EulerAngles);


        // Calculate offset
        XMVECTOR offset =
            XMVector3Rotate(m_CurrentVelocity * deltaTime, _cachedControlledTransformComponent->transform.rotation.data);
        offset = XMVector3Normalize(XMVectorSetY(offset, 0.0f)) * XMVectorGetX(XMVector3Length(offset));
        _cachedControlledTransformComponent->transform.translation += offset;
}

void PlayerController::UpdateOnPuzzle()
{
        switch (m_CurrentPuzzleState)
        {
                case EPuzzleState::EASE_IN:
                {
                        break;
                }
                case EPuzzleState::EASE_OUT:
                {
                        break;
                }
                case EPuzzleState::SOLVE:
                {
                        break;
                }
                default:
                {
                        assert(false && "Wrong Puzzle State passed");
                }
        }
}

void PlayerController::UpdateOnPuzzleEaseIn()
{
        GoalComponent*      goalComp = GEngine::Get()->GetComponentManager()->GetComponent<GoalComponent>(m_GoalComponent);

		//goalComp->initialTransform.translation

        TransformComponent* goalTransform =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(goalComp->GetOwner());
}

void PlayerController::UpdateOnPuzzleEaseOut()
{
        GoalComponent*      goalComp = GEngine::Get()->GetComponentManager()->GetComponent<GoalComponent>(m_GoalComponent);
        TransformComponent* goalTransform =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(goalComp->GetOwner());
}

void PlayerController::UpdateOnPuzzleSolve()
{
        GoalComponent*      goalComp = GEngine::Get()->GetComponentManager()->GetComponent<GoalComponent>(m_GoalComponent);
        TransformComponent* goalTransform =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(goalComp->GetOwner());
}

PlayerController::PlayerController()
{
        m_CurrentVelocity = DirectX::XMVectorZero();
        m_CurrentPosition = DirectX::XMVectorZero();
        m_CurrentInput    = DirectX::XMVectorZero();
        m_MouseXDelta     = 0;
        m_MouseYDelta     = 0;
}

void PlayerController::Init(EntityHandle h)
{
        IController::Init(h);
        TransformComponent* transformComp =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_ControlledEntityHandle);

        m_EulerAngles = transformComp->transform.rotation.ToEulerAngles();
}

void PlayerController::SpeedBoost(DirectX::XMVECTOR preBoostVelocity)
{
        preBoostVelocity = m_CurrentVelocity;
        m_CurrentVelocity += 2.0f * XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        maxMaxSpeed += 1;
}
