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

#include "PlayerGroundState.h"

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

        m_CurrentInput = XMVectorZero();
        if (IsEnabled())
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
        m_StateMachine.Update(cacheTime, _cachedControlledTransformComponent);
        FSphere fSpherePlayer;
        fSpherePlayer.center = _cachedControlledTransformComponent->transform.translation;
        fSpherePlayer.radius = 0.25f;

        debug_renderer::AddSphere(fSpherePlayer, 36, XMMatrixIdentity());
}

PlayerController::PlayerController()
{
        m_CurrentVelocity = DirectX::XMVectorZero();
        m_CurrentInput    = DirectX::XMVectorZero();
}

void PlayerController::Init(EntityHandle h)
{
        IController::Init(h);
        TransformComponent* transformComp =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_ControlledEntityHandle);

        m_EulerAngles = transformComp->transform.rotation.ToEulerAngles();

		// Create any states and set their respective variables here
		m_StateMachine.CreateState<PlayerGroundState>();

		// After you create the states, initialize the state machine. First created state is starting state
        m_StateMachine.Init(this);
}

void PlayerController::SpeedBoost(DirectX::XMVECTOR preBoostVelocity)
{
        preBoostVelocity = m_CurrentVelocity;
        m_CurrentVelocity += 2.0f * XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        maxMaxSpeed += 1;
}
