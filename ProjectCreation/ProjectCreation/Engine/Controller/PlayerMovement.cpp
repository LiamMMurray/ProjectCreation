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
using namespace CollisionComponent;
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

        // Convert the angle of change on the X-Axis and the Y-Axis to radians
        static float accumAngleY = 0.f;
        float        angleX      = XMConvertToRadians(m_MouseXDelta * 10.f);
        float        angleY      = XMConvertToRadians(m_MouseYDelta * 10.f);
        float        angleZ      = XMConvertToRadians(m_MouseXDelta * 5.0f);

        if (angleZ >= XMConvertToRadians(20.0f))
        {
                angleZ = XMConvertToRadians(20.0f);
        }

        if (angleZ <= XMConvertToRadians(-20.0f))
        {
                angleZ = XMConvertToRadians(-20.0f);
        }

        // Set the local X-Axis, Y-Axis and Z-Axis
        XMVECTOR YAxis = VectorConstants::Up;
        XMVECTOR XAxis = transformComp->transform.GetRight();
        XMVECTOR ZAxis = transformComp->transform.GetForward();

        // Get the rotation of the transform component
        FQuaternion rot = transformComp->transform.rotation;

        // Get horizontal rotation (Rotate around the Y)
        FQuaternion horizontalRot = FQuaternion::RotateAxisAngle(YAxis, angleX);

        // Get the vertical rotation (Rotate around the X)
        FQuaternion verticalRot = FQuaternion::RotateAxisAngle(XAxis, angleY);

        // Get the normal rotation (Rotate around the Z)
        FQuaternion normalRot = FQuaternion::RotateAxisAngle(ZAxis, angleZ);

        // Get the old forward
        XMVECTOR prevFw = XMVectorSetY(rot.GetForward(), 0.0f);

        // Normalize the old forward
        prevFw = XMVector3Normalize(prevFw);

        // Calculate the new forward
        XMVECTOR nextFw = XMVector3Rotate(rot.GetForward(), verticalRot.rotation);

        // Calculate the angle between the old forward and new forward
        XMVECTOR angleVec = XMVector3AngleBetweenVectors(prevFw, nextFw);

        // Convert the angle between the old forward and the new forward to degrees
        float angle = XMConvertToDegrees(XMVectorGetX(angleVec));

        // Check if the converted angle between forwards is greather that 90 degrees
        if (angle > 90.f)
        {
                // Set vertical rotation to the identity
                verticalRot = XMQuaternionIdentity();
        }

        // Calculate offset
        XMVECTOR offset = XMVector3Rotate(m_CurrentVelocity * deltaTime, transformComp->transform.rotation.rotation);
        offset          = XMVector3Normalize(XMVectorSetY(offset, 0.0f)) * XMVectorGetX(XMVector3Length(offset));

        FSphere fSphereStart;
        fSphereStart.center = transformComp->transform.translation;
        fSphereStart.radius = 0.2f;
        FSphere fSphereEnd;
        fSphereEnd.center = transformComp->transform.translation + offset;
        fSphereEnd.radius = 0.2f;
        FSphere fSphereCheck;
        fSphereCheck.center = XMVectorSet(0.0f, 0.0f, 0.f, 1.0f);
        fSphereCheck.radius = 0.2f;


         AddSphere(fSphereStart, 36, XMMatrixIdentity());
        // AddSphere(fSphereEnd, 36, XMMatrixIdentity());
        // AddSphere(fSphereCheck, 36, XMMatrixIdentity());

        FSweepCollisionResult result = CollisionLibary::SweepSphereToSphere(fSphereStart, fSphereEnd, fSphereCheck, 1.0f);

        if (result.collisionType != CollisionComponent::ECollisionType::EOveralap &&
            result.collisionType != CollisionComponent::ECollisionType::ECollide)
        {
                transformComp->transform.translation += offset;
        }
        else
        {
                m_CurrentVelocity                    = m_CurrentVelocity - deltaVec * delta;
                transformComp->transform.translation = result.finalPosition + m_CurrentVelocity;
        }
        transformComp->transform.rotation = transformComp->transform.rotation * verticalRot * horizontalRot * normalRot;

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

        ApplyInput();
}

PlayerController::PlayerController()
{
        m_CurrentVelocity = DirectX::XMVectorZero();
        m_CurrentPosition = DirectX::XMVectorZero();
        m_MouseXDelta     = 0;
        m_MouseYDelta     = 0;
}
