#include "PlayerMovement.h"
#include "../Components/PhysicsComponent.h"
#include "../CoreInput/CoreInput.h"
#include "../ECS/Entity.h"
#include "../Engine/GEngine.h"
#include "../ECS/Entity.h"
#include "../Components/PhysicsComponent.h"

// v Testing only delete when done v
#include <iostream>
// ^ Testing only delete when done ^

#include "../MathLibrary/MathLibrary.h"
#include "../MathLibrary/Quaternion.h"

// v Testing only delete when done v
#include <iostream>
// ^ Testing only delete when done ^
using namespace DirectX;

template <typename T>
inline T lerp(T v0, T v1, T t)
{
        return fma(t, v1, fma(-t, v0, v0));
}


template <typename T>
inline T clamp(const T& n, const T& lower, const T& upper)
{
        return max(lower, min(n, upper));
}

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
        float    maxSpeed    = lerp(minMaxSpeed, maxMaxSpeed, currForward - minMaxSpeed);

        if (fabs(currSpeed) > fabs(maxSpeed))
                currSpeed = clamp(currSpeed, -maxSpeed, maxSpeed);

        XMVECTOR desiredVelocity = currSpeed * desiredDir;

        float accel = (XMVectorGetX(XMVector3Length(desiredVelocity)) >= XMVectorGetX(XMVector3Length(m_CurrentVelocity))) ?
                          acceleration :
                          deacceleration;

        float dist = MathLibrary::CalulateDistance(m_CurrentVelocity, desiredVelocity);

        float delta = min(accel * deltaTime, dist);

        XMVECTOR deltaVec = XMVector3Normalize(desiredVelocity - m_CurrentVelocity);

        m_CurrentVelocity = m_CurrentVelocity + deltaVec * delta;



        m_MouseXDelta                 = static_cast<float>(GCoreInput::GetMouseX());
        m_MouseYDelta                 = static_cast<float>(GCoreInput::GetMouseY());

        static float accumAngleY = 0.f;
        float        angleX      = XMConvertToRadians(m_MouseXDelta * 10.f);
        float        angleY      = XMConvertToRadians(m_MouseYDelta * 10.f);
        ;

        XMVECTOR YAxis = transformComponent.transform.GetUp();
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

		


        transformComponent.transform.translation += XMVector3Rotate(m_CurrentVelocity * deltaTime, transformComponent.transform.rotation.rotation);
        transformComponent.transform.rotation = transformComponent.transform.rotation * verticalRot * horizontalRot;
        //std::cout << accel << std::endl;

        //std::cout << "Current Position < " << XMVectorGetX(transformComponent.transform.translation) << ", "
        //          << XMVectorGetY(transformComponent.transform.translation) << ", "
        //          << XMVectorGetZ(transformComponent.transform.translation) << ", "
        //          << XMVectorGetW(transformComponent.transform.translation) << " >" << std::endl;

        std::cout << "Current Forward < " << XMVectorGetX(nextFw) << ", "
                  << XMVectorGetY(nextFw) << ", "
                  << XMVectorGetZ(nextFw) << ", "
                  << XMVectorGetW(nextFw) << " >" << std::endl;
}

PlayerMovement::PlayerMovement()
{
        m_CurrentVelocity = DirectX::XMVectorZero();
        m_MouseXDelta     = 0;
        m_MouseYDelta     = 0;
}

PhysicsComponent   PlayerMovement::physicsComponent;
TransformComponent PlayerMovement::transformComponent;
