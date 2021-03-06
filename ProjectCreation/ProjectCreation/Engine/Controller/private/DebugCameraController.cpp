#include "DebugCameraController.h"
#include <Entity.h>
#include <CoreInput.h>
#include <GEngine.h>

#include <MathLibrary.h>
#include <Quaternion.h>

#include <debug_renderer.h>
#include <ControllerSystem.h>
#include <CollisionLibary.h>

#include <CollisionShapes.h>

// v Testing only delete when done v
#include <iostream>
// ^ Testing only delete when done ^
using namespace DirectX;
using namespace Shapes;
using namespace debug_renderer;

void DebugCameraController::GatherInput()
{
        if (!IsEnabled())
        {
                auto eHandle = SYSTEM_MANAGER->GetSystem<ControllerSystem>()->GetCurrentController()->GetControlledEntity();
                auto controllerTransform = eHandle.GetComponent<TransformComponent>();

                TransformComponent* myTransform = m_ControlledEntityHandle.GetComponent<TransformComponent>();

                myTransform->transform = controllerTransform->transform;
                GEngine::Get()->SetDebugMode(false);

                m_EulerAngles = controllerTransform->transform.rotation.ToEulerAngles();

                return;
        }

        GEngine::Get()->SetDebugMode(true);

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

void DebugCameraController::ProcessInput()
{}

void DebugCameraController::ApplyInput()
{
        if (!IsEnabled())
                return;

        // Get the Transoform Component
        TransformComponent* transformComp = m_ControlledEntityHandle.GetComponent<TransformComponent>();

        // Get Delta Time
        float deltaTime = cacheTime; // GEngine::Get()->GetDeltaTime();

        // Get the Speed from the gathered input
        float currSpeed = XMVectorGetX(XMVector3Length(m_CurrentInput));

        // Get the Forward from the gathered input
        float currForward = XMVectorGetZ(m_CurrentInput);

        // Normalize the gathered input to determine the desired direction
        XMVECTOR desiredDir = XMVector3Normalize(m_CurrentInput);

        // Determine the max speed the object can move
        float maxSpeed = 10.0f;

        if (GCoreInput::GetMouseState(MouseCode::RightClick) == KeyState::Down)
                maxSpeed *= 10.0f;

        float angularSpeed = XMConvertToRadians(2.0f) * deltaTime;
        m_EulerAngles.y += m_MouseXDelta * angularSpeed;
        m_EulerAngles.x += m_MouseYDelta * angularSpeed;
        constexpr float pitchLimit = XMConvertToRadians(90.0f);
        m_EulerAngles.x            = MathLibrary::clamp(m_EulerAngles.x, -pitchLimit, pitchLimit);
        constexpr float rollLimit  = XMConvertToRadians(20.0f);
        m_EulerAngles.z            = 0.0f;

        // Calculate offset
        XMVECTOR offset = XMVector3Rotate(m_CurrentInput * deltaTime * maxSpeed, transformComp->transform.rotation.data);

        transformComp->transform.rotation = FQuaternion::FromEulerAngles(m_EulerAngles);
        // Set the rotation for the transform component
        transformComp->transform.translation += offset;

        // Write out information to the console window
        // std::cout << "Current Forward < " << XMVectorGetX(nextFw) << ", " << XMVectorGetY(nextFw) << ", "
        //          << XMVectorGetZ(nextFw) << ", " << XMVectorGetW(nextFw) << " >" << std::endl;
}

DebugCameraController::DebugCameraController()
{
        m_CurrentVelocity = DirectX::XMVectorZero();
        m_MouseXDelta     = 0;
        m_MouseYDelta     = 0;
}
