#include "PlayerMovement.h"
#include "../Components/PhysicsComponent.h"
#include "../CoreInput/CoreInput.h"
#include "../ECS/Entity.h"
#include "../Engine/GEngine.h"

#include "../MathLibrary/MathLibrary.h"
#include "../MathLibrary/Quaternion.h"

#include "../CollisionLibary/CollisionLibary.h"

// v Testing only delete when done v
#include <iostream>
// ^ Testing only delete when done ^
using namespace DirectX;


void PlayerMovement::GatherInput()
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

        m_MouseXDelta = GCoreInput::GetMouseX();
        m_MouseYDelta = GCoreInput::GetMouseY();

        m_CurrentInput = XMLoadFloat4(&tempDir);
}

void PlayerMovement::ProcessInput()
{}

void PlayerMovement::ApplyInput()
{
        float deltaTime = GEngine::Get()->GetDeltaTime();

        float    currSpeed   = XMVectorGetX(XMVector3Length(m_CurrentInput));
        float    currForward = XMVectorGetZ(m_CurrentInput);
        XMVECTOR desiredDir  = XMVector3Normalize(m_CurrentInput);
        float    maxSpeed    = MathLibrary::lerp(minMaxSpeed, maxMaxSpeed, currForward - minMaxSpeed);

        if (fabs(currSpeed) > fabs(maxSpeed))
                currSpeed = MathLibrary::clamp(currSpeed, -maxSpeed, maxSpeed);

        XMVECTOR desiredVelocity = currSpeed * desiredDir;

        float accel = (XMVectorGetX(XMVector3Length(desiredVelocity)) >= XMVectorGetX(XMVector3Length(m_CurrentVelocity))) ?
                          acceleration :
                          deacceleration;

        float dist = MathLibrary::CalulateDistance(m_CurrentVelocity, desiredVelocity);

        float delta = std::min(accel * deltaTime, dist);

        XMVECTOR deltaVec = XMVector3Normalize(desiredVelocity - m_CurrentVelocity);

        m_CurrentVelocity = m_CurrentVelocity + deltaVec * delta;


        m_MouseXDelta = (GCoreInput::GetMouseX());
        m_MouseYDelta = (GCoreInput::GetMouseY());

        static float accumAngleY = 0.f;
        float        angleX      = XMConvertToRadians(m_MouseXDelta * 10.f);
        float        angleY      = XMConvertToRadians(m_MouseYDelta * 10.f);
        ;

        XMVECTOR YAxis = VectorConstants::Up;
        XMVECTOR XAxis = transformComponent.transform.GetRight();

        FQuaternion rot           = transformComponent.transform.rotation;
        FQuaternion horizontalRot = FQuaternion::RotateAxisAngle(YAxis, angleX);
        FQuaternion verticalRot   = FQuaternion::RotateAxisAngle(XAxis, angleY);

        XMVECTOR prevFw   = XMVectorSetY(rot.GetForward(), 0.0f);
        prevFw            = XMVector3Normalize(prevFw);
        XMVECTOR nextFw   = XMVector3Rotate(rot.GetForward(), verticalRot.rotation);
        XMVECTOR angleVec = XMVector3AngleBetweenVectors(prevFw, nextFw);
        float    angle    = XMConvertToDegrees(XMVectorGetX(angleVec));
        if (angle > 90.f)
                verticalRot = XMQuaternionIdentity();

        XMVECTOR offset = XMVector3Rotate(m_CurrentVelocity * deltaTime, transformComponent.transform.rotation.rotation);
        offset          = XMVector3Normalize(XMVectorSetY(offset, 0.0f)) * XMVectorGetX(XMVector3Length(offset));

        CollisionComponent::FSphere fSphereStart;
        fSphereStart.center = transformComponent.transform.translation;
        fSphereStart.radius = 0.2f;
        CollisionComponent::FSphere fSphereEnd;
        fSphereEnd.center = transformComponent.transform.translation + offset;
        fSphereEnd.radius = 0.2f;
        CollisionComponent::FSphere fSphereCheck;
        fSphereCheck.center = XMVectorSet(0.0f, 0.0f, 0.f, 1.0f);
        fSphereCheck.radius = 0.2f;

        CollisionComponent::FCollideResult result =
            CollisionLibary::SweepSphereToSphere(fSphereStart, fSphereEnd, fSphereCheck);

        if (result.collisionType != CollisionComponent::ECollisionType::EOveralap &&
            result.collisionType != CollisionComponent::ECollisionType::ECollide)
        {
                transformComponent.transform.translation += offset;
        }
		else
		{
                // m_CurrentVelocity = m_CurrentVelocity - deltaVec * delta;
		}
        transformComponent.transform.rotation = transformComponent.transform.rotation * verticalRot * horizontalRot;
        // std::cout << accel << std::endl;

        // std::cout << "Current Position < " << XMVectorGetX(transformComponent.transform.translation) << ", "
        //          << XMVectorGetY(transformComponent.transform.translation) << ", "
        //          << XMVectorGetZ(transformComponent.transform.translation) << ", "
        //          << XMVectorGetW(transformComponent.transform.translation) << " >" << std::endl;

        std::cout << "Current Forward < " << XMVectorGetX(nextFw) << ", " << XMVectorGetY(nextFw) << ", "
                  << XMVectorGetZ(nextFw) << ", " << XMVectorGetW(nextFw) << " >" << std::endl;
}

PlayerMovement::PlayerMovement()
{
        m_CurrentVelocity                        = DirectX::XMVectorZero();
        m_MouseXDelta                            = 0;
        m_MouseYDelta                            = 0;
        transformComponent.transform.translation = XMVectorSet(0.0f, 0.3f, -2.0f, 1.0f);
}

PhysicsComponent   PlayerMovement::physicsComponent;
TransformComponent PlayerMovement::transformComponent;
