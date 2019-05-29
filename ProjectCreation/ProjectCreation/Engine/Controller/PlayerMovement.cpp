#include "PlayerMovement.h"
#include "../../ECS/Entity.h"
#include "../CoreInput/CoreInput.h"
#include "../GEngine.h"

#include "../MathLibrary/MathLibrary.h"
#include "../MathLibrary/Quaternion.h"

#include "../CollisionLibary/CollisionLibary.h"
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
        PastDirection = requestedDirection;

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

void PlayerController::ProcessInput()
{}

void PlayerController::ApplyInput()
{
        // Get the Transoform Component
        TransformComponent* transformComp =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_ControlledEntityHandle);


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

        if (GCoreInput::GetKeyState(KeyCode::K) == KeyState::DownFirst)
        {
                preBoostVelocity = m_CurrentVelocity;
                m_CurrentVelocity += 10.0f * XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        }

        m_CurrentVelocity =
            XMVectorLerp(m_CurrentVelocity, preBoostVelocity, MathLibrary::clamp(deltaTime * 0.5f, 0.0f, 1.0f));

        // Convert the angle of change on the X-Axis and the Y-Axis to radians
        static float accumAngleY = 0.0f;
        m_Yaw += m_MouseXDelta * 5.0f * deltaTime;
        m_Pitch += m_MouseYDelta * 5.0f * deltaTime;
        m_Pitch = MathLibrary::clamp(m_Pitch, -90.0f, 90.0f);
        m_Roll += m_MouseXDelta * 4.0f * deltaTime;
        m_Roll = MathLibrary::clamp(m_Roll, -20.0f, 20.0f);

        m_Roll = MathLibrary::lerp(m_Roll, 0.0f, MathLibrary::clamp(deltaTime * 20.0f, 0.0f, 1.0f));

        // Calculate offset
        XMVECTOR offset = XMVector3Rotate(m_CurrentVelocity * deltaTime, transformComp->transform.rotation.data);
        offset          = XMVector3Normalize(XMVectorSetY(offset, 0.0f)) * XMVectorGetX(XMVector3Length(offset));

        FSphere fSphereStart;
        fSphereStart.center = transformComp->transform.translation;
        fSphereStart.radius = 0.2f;
        FSphere fSphereEnd;
        fSphereEnd.center = transformComp->transform.translation + offset;
        fSphereEnd.radius = 0.2f;
        FSphere fSphereCheck;
        fSphereCheck.center = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        fSphereCheck.radius = 0.2f;


		//AddSphere(fSphereStart,36,XMMatrixIdentity());
       // AddSphere(fSphereEnd, 36, XMMatrixIdentity());
       // AddSphere(fSphereCheck, 36, XMMatrixIdentity());
		
		FAdvancedviCollisionResult result =
            CollisionLibary::SweepSphereToSphere(fSphereStart, fSphereEnd, fSphereCheck, 1.0f);

        if (result.collisionType != Collision::ECollisionType::EOveralap &&
            result.collisionType != Collision::ECollisionType::ECollide)
        {
                transformComp->transform.translation += offset;
        }
        else
        {
                m_CurrentVelocity                    = m_CurrentVelocity - deltaVec * delta;
                transformComp->transform.translation = result.finalPosition + m_CurrentVelocity;
        }
        transformComp->transform.rotation = XMQuaternionRotationRollPitchYaw(
            XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), XMConvertToRadians(m_Roll));

        m_CurrentPosition = transformComp->transform.translation;

        // Write out information to the console window
        //  std::cout << "Current Forward < " << XMVectorGetX(nextFw) << ", " << XMVectorGetY(nextFw) << ", "
        //            << XMVectorGetZ(nextFw) << ", " << XMVectorGetW(nextFw) << " >" << std::endl;
}

void PlayerController::PauseInput()
{}


void PlayerController::InactiveUpdate(float deltaTime)
{
        FSphere fSpherePlayer;
        fSpherePlayer.center = m_CurrentPosition;
        fSpherePlayer.radius = 0.25f;

        debug_renderer::AddSphere(
            fSpherePlayer, 36, XMMatrixMultiply(XMMatrixIdentity(), XMMatrixTranslationFromVector(m_CurrentPosition)));

        //ApplyInput();

        // Get the Transoform Component
        TransformComponent* transformComp =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_ControlledEntityHandle);


        // Get Delta Time
        float DeltaTime = cacheTime; // GEngine::Get()->GetDeltaTime();

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
        float accel = deacceleration;

        // Calculate distance from our current velocity to our desired velocity
        float dist = MathLibrary::CalulateDistance(m_CurrentVelocity, desiredVelocity);

        // Calculate change based on the type of acceleration, the change in time, and the calculated distance
        float delta = std::min(accel * DeltaTime, dist);

        // Normalize the difference of the desired velocity and the current velocity
        XMVECTOR deltaVec = XMVector3Normalize(desiredVelocity - m_CurrentVelocity);

        // Calculate current velocity based on itself, the deltaVector, and delta
        m_CurrentVelocity = m_CurrentVelocity + deltaVec * delta;
}

PlayerController::PlayerController()
{
        m_CurrentVelocity = DirectX::XMVectorZero();
        m_CurrentPosition = DirectX::XMVectorZero();
        m_MouseXDelta     = 0;
        m_MouseYDelta     = 0;
}
