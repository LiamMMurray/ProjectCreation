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

#include "PlayerCinematicState.h"
#include "PlayerGroundState.h"
#include "PlayerPuzzleState.h"
#include "PlayerStateEvents.h"

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
                        //tempDir.z += 1.0f;
                }

                if (GCoreInput::GetKeyState(KeyCode::E) == KeyState::Down)
                {
                        //tempDir.z += 1.0f;
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
        m_CinematicState = m_StateMachine.CreateState<PlayerCinematicState>();
        auto groundState = m_StateMachine.CreateState<PlayerGroundState>();
        auto puzzleState = m_StateMachine.CreateState<PlayerPuzzleState>();

        m_StateMachine.AddTransition(groundState, m_CinematicState, E_PLAYERSTATE_EVENT::TO_TRANSITION);
        m_StateMachine.AddTransition(m_CinematicState, groundState, E_PLAYERSTATE_EVENT::TO_GROUND);
        m_StateMachine.AddTransition(m_CinematicState, puzzleState, E_PLAYERSTATE_EVENT::TO_PUZZLE);

        m_StateMachine.AddTransition(groundState, puzzleState, E_PLAYERSTATE_EVENT::TO_PUZZLE);
        m_StateMachine.AddTransition(puzzleState, groundState, E_PLAYERSTATE_EVENT::TO_GROUND);
        m_StateMachine.AddTransition(puzzleState, m_CinematicState, E_PLAYERSTATE_EVENT::TO_TRANSITION);

        // Request initial transition
        FTransform target = transformComp->transform;
        target.rotation   = XMQuaternionIdentity();
        RequestCinematicTransition(1, &transformComp->GetHandle(), &target, E_PLAYERSTATE_EVENT::TO_GROUND, 5.0f);

        // After you create the states, initialize the state machine. First created state is starting state
        m_StateMachine.Init(this);
}

void PlayerController::SpeedBoost(DirectX::XMVECTOR preBoostVelocity)
{
        preBoostVelocity = m_CurrentVelocity;
        m_CurrentVelocity += 2.0f * XMVector3Normalize(m_CurrentVelocity);
}

void PlayerController::RequestCinematicTransition(int                    count,
                                                  const ComponentHandle* handles,
                                                  const FTransform*      targets,
                                                  int                    targetState,
                                                  float                  duration,
                                                  float                  delay)
{
        TransformComponent* transformComp =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_ControlledEntityHandle);

        m_CinematicState->SetTransitionMode(E_TRANSITION_MODE::Simple);
        m_CinematicState->AddTransformTransitions(count, handles, targets);
        m_CinematicState->SetTansitionTargetState(targetState);
        m_CinematicState->SetTansitionDuration(duration);
        m_CinematicState->SetTransitionDelay(delay);

        m_StateMachine.Transition(E_PLAYERSTATE_EVENT::TO_TRANSITION);
}

void PlayerController::RequestCinematicTransitionLookAt(const ComponentHandle  lookAtTarget,
                                                        int                    count,
                                                        const ComponentHandle* handles,
                                                        const FTransform*      targets,
                                                        int                    targetState,
                                                        float                  duration,
                                                        float                  lookAtTransitionDuration,
                                                        float                  delay)
{
        TransformComponent* transformComp =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_ControlledEntityHandle);

        m_CinematicState->SetTransitionMode(E_TRANSITION_MODE::LookAt);
        m_CinematicState->AddTransformTransitions(count, handles, targets);
        m_CinematicState->SetLookAtTransitionDuration(lookAtTransitionDuration);
        m_CinematicState->SetLookAtTarget(lookAtTarget);
        m_CinematicState->SetTansitionTargetState(targetState);
        m_CinematicState->SetTansitionDuration(duration);
        m_CinematicState->SetTransitionDelay(delay);

        m_StateMachine.Transition(E_PLAYERSTATE_EVENT::TO_TRANSITION);
}

void PlayerController::RequestPuzzleMode(ComponentHandle          goalHandle,
                                         const DirectX::XMVECTOR& puzzleCenter,
                                         bool                     alignToGoal,
                                         float                    transitionDuration,
                                         float                    lookAtDuration,
                                         int                      otherTransformsCount,
                                         const ComponentHandle*   handles,
                                         const FTransform*        transforms)
{
        SetGoalComponent(goalHandle);

        if (!alignToGoal)
                m_StateMachine.Transition(E_PLAYERSTATE_EVENT::TO_PUZZLE);
        else
        {
                auto playerTransformComp =
                    GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_ControlledEntityHandle);

                GoalComponent*  goalComp = GEngine::Get()->GetComponentManager()->GetComponent<GoalComponent>(goalHandle);
                ComponentHandle goalTransformHandle =
                    GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(goalComp->GetOwner())->GetHandle();

                FSphere sphereInitial;
                sphereInitial.center = goalComp->initialTransform.translation;
                sphereInitial.radius = goalComp->initialTransform.GetRadius();

                FSphere sphereTarget;
                sphereTarget.center = goalComp->goalTransform.translation;
                sphereTarget.radius = goalComp->goalTransform.GetRadius();

                XMVECTOR eyePos = playerTransformComp->transform.translation;

                float desiredAngularDiameter = MathLibrary::CalculateAngularDiameter(eyePos, sphereTarget);
                float desiredInitialDistance =
                    MathLibrary::CalculateDistanceFromAngularDiameter(desiredAngularDiameter, sphereInitial);

                XMVECTOR dir = XMVector3Normalize(puzzleCenter - eyePos);

                FTransform targetGoalTransform  = goalComp->initialTransform;
                targetGoalTransform.translation = eyePos + dir * desiredInitialDistance;

                m_CinematicState->AddTransformTransitions(1, &goalTransformHandle, &targetGoalTransform);
                RequestCinematicTransitionLookAt(goalTransformHandle,
                                                 otherTransformsCount,
                                                 handles,
                                                 transforms,
                                                 E_PLAYERSTATE_EVENT::TO_PUZZLE,
                                                 transitionDuration,
                                                 lookAtDuration);
        }
}
