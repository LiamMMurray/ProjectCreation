#include "PlayerMovement.h"
#include "../../ECS/Entity.h"
#include "..//Gameplay/GoalComponent.h"
#include "..//Gameplay/OrbitSystem.h"
#include "../CollisionLibary/CollisionLibary.h"
#include "../CollisionLibary/CollisionResult.h"
#include "../CoreInput/CoreInput.h"
#include "../GEngine.h"
#include "../GenericComponents/TransformComponent.h"
#include "../MathLibrary/MathLibrary.h"
#include "../MathLibrary/Quaternion.h"
#include "ControllerSystem.h"

#include "PlayerCinematicState.h"
#include "PlayerGroundState.h"
#include "PlayerPuzzleState.h"
#include "PlayerStateEvents.h"

#include "../../Rendering/DebugRender/debug_renderer.h"
#include "..//Audio/ContinousSoundSystem.h"
// v Testing only delete when done v
#include <iostream>
#include "../CoreInput/InputActions.h"
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
                XMFLOAT4 tempDir = {0.0f, 0.0f, 0.0f, 0.0f};

                {
                        if (GamePad::Get()->CheckConnection() == true)
                        {
                                tempDir.z += (1.5 * GamePad::Get()->rightTrigger);
                        }

                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::Down)
                        {
                                tempDir.z += 1.5f;
                        }
                }

                m_CurrentInput = XMLoadFloat4(&tempDir);
        }
}

void PlayerController::ProcessInput()
{
        _cachedControlledTransform = m_ControlledEntityHandle.GetComponent<TransformComponent>()->transform;
}

void PlayerController::ApplyInput()
{
        m_StateMachine.Update(cacheTime, _cachedControlledTransform);
        FSphere fSpherePlayer;
        fSpherePlayer.center = _cachedControlledTransform.translation;
        fSpherePlayer.radius = 0.25f;
#ifdef _DEBUG
        debug_renderer::AddSphere(fSpherePlayer, 4, XMMatrixIdentity());
#endif
}

void PlayerController::DebugPrintSpeedBoostColor(int color)
{
        switch (color)
        {
                case 0:
                        ConsoleWindow::PrintMessage("Boosting on red light", "PlayerMovement");
                        break;
                case 1:
                        ConsoleWindow::PrintMessage("Boosting on green light", "PlayerMovement");
                        break;
                case 2:
                        ConsoleWindow::PrintMessage("Boosting on blue light", "PlayerMovement");
                        break;
                case 3:
                        ConsoleWindow::PrintMessage("Boosting on white light", "PlayerMovement");
                        break;
        }
}

void PlayerController::IncreaseCollectedSplineOrbCount(int color)
{
        m_CollectedSplineOrbCount++;

        if (m_CollectedSplineOrbCount >= m_TotalSplineOrbCount * 0.7f)
        {
                m_CollectedSplineOrbCount = 0;
                GET_SYSTEM(OrbitSystem)->m_PendingGoalCounts[color]++;
        }
}

void PlayerController::Shutdown()
{
        m_StateMachine.Shutdown();
}

void PlayerController::Init(EntityHandle h)
{
        IController::Init(h);
        auto tComp = h.GetComponent<TransformComponent>();

        tComp->transform.translation = DirectX::XMVectorSet(0.0f, 3.0f, 0.0f, 1.0f);
        tComp->transform.rotation    = DirectX::XMQuaternionRotationRollPitchYaw(DirectX::XMConvertToRadians(0.0f), 0.0f, 0.0f);

        m_CurrentVelocity       = DirectX::XMVectorZero();
        m_CurrentInput          = DirectX::XMVectorZero();
        ComponentHandle tHandle = m_ControlledEntityHandle.GetComponentHandle<TransformComponent>();

        SetSensitivity(1);

        m_EulerAngles = tComp->transform.rotation.ToEulerAngles();

        // Collecting spline orb values
        m_TotalSplineOrbCount     = 0;
        m_CollectedSplineOrbCount = 0;

        m_StateMachine.Shutdown();
        // Create any states and set their respective variables here
        m_CinematicState = m_StateMachine.CreateState<PlayerCinematicState>();
        m_GroundState    = m_StateMachine.CreateState<PlayerGroundState>();
        auto puzzleState = m_StateMachine.CreateState<PlayerPuzzleState>();

        m_StateMachine.AddTransition(m_GroundState, m_CinematicState, E_PLAYERSTATE_EVENT::TO_TRANSITION);
        m_StateMachine.AddTransition(m_CinematicState, m_GroundState, E_PLAYERSTATE_EVENT::TO_GROUND);
        m_StateMachine.AddTransition(m_CinematicState, puzzleState, E_PLAYERSTATE_EVENT::TO_PUZZLE);

        m_StateMachine.AddTransition(m_GroundState, puzzleState, E_PLAYERSTATE_EVENT::TO_PUZZLE);
        m_StateMachine.AddTransition(puzzleState, m_GroundState, E_PLAYERSTATE_EVENT::TO_GROUND);
        m_StateMachine.AddTransition(puzzleState, m_CinematicState, E_PLAYERSTATE_EVENT::TO_TRANSITION);

        // Request initial transition
        FTransform target = tComp->transform;
        target.rotation   = XMQuaternionIdentity();


        // After you create the states, initialize the state machine. First created state is starting state
        RequestCinematicTransition(1, &tHandle, &target, E_PLAYERSTATE_EVENT::TO_GROUND, 1.0f);
        m_StateMachine.Init(this);


        // Init sound pool
}

void PlayerController::Reset()
{
        Init(m_ControlledEntityHandle);
}

bool PlayerController::SpeedBoost(DirectX::XMVECTOR boostPos, int color)
{
        // Audio that will play on boost


        if ((int)m_ColorInputKeyCodes[color] < 0 || InputActions::CheckAction(color) == KeyState::Down)
        {

                // Settings for the orb sounds (Referencing SpeedBoostSystem.cpp lines 814-846
                int index = SYSTEM_MANAGER->GetSystem<ControllerSystem>()->GetOrbCount();
                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->IncreaseOrbCount(color);

                if (color == 3)
                {
                        AudioManager::Get()->PlaySoundWithVolume(1.0f, m_SpeedboostSoundNames[index].c_str());
                }
                else
                {
                        SoundComponent3D::FSettings settings;
                        settings.m_SoundType = SOUND_COLOR_TYPE(color, E_SOUND_TYPE::ORB_COLLECT_0);

                        int totalVariations = E_SOUND_TYPE::variations[E_SOUND_TYPE::ORB_COLLECT_0];
                        int variation       = index % (totalVariations);

                        if (variation >= E_SOUND_TYPE::variations[E_SOUND_TYPE::ORB_COLLECT_0])
                        {
                                variation = totalVariations - variation;
                        }

                        settings.m_SoundVaration = variation;
                        settings.flags.set(SoundComponent3D::E_FLAGS::DestroyOnEnd, true);
                        settings.m_Volume = 1.0f;

                        AudioManager::Get()->PlaySoundAtLocation(boostPos, settings);
                }

                // bool isPlaying;
                // m_SpeedBoostSoundPool[color][m_SpeedBoostPoolCounter[color]]->isSoundPlaying(isPlaying);
                // m_SpeedBoostSoundPool[color][m_SpeedBoostPoolCounter[color]]->Play();
                // DebugPrintSpeedBoostColor(color);
                currentMaxSpeed       = std::min(currentMaxSpeed + 0.5f, maxMaxSpeed);
                XMVECTOR currentInput = XMVector3Rotate(m_CurrentInput, _cachedControlledTransform.rotation.data);
                if (MathLibrary::VectorDotProduct(currentInput, m_CurrentVelocity) > 0.0f)
                {
                        m_CurrentVelocity += 1.0f * XMVector3Normalize(m_CurrentVelocity);
                        m_CurrentVelocity = XMVector3ClampLength(m_CurrentVelocity, 0.0f, currentMaxSpeed);
                        m_GroundState->AddSpeedBoost();
                }
                m_SpeedBoostPoolCounter[color]++;
                m_SpeedBoostPoolCounter[color] %= MAX_SPEEDBOOST_SOUNDS;


                return true;
        }
        return false;
}

void PlayerController::AddCurrentVelocity(DirectX::XMVECTOR val)
{
        m_CurrentVelocity = m_CurrentVelocity + val;
}

void PlayerController::SetUseGravity(bool val)
{
        m_GroundState->bUseGravity = val;
}

bool PlayerController::GetUseGravity()
{
        return m_GroundState->bUseGravity;
}

void PlayerController::SetYExtraSpeed(float val)
{
        m_GroundState->m_ExtraYSpeed = val;
}

void PlayerController::RequestCinematicTransition(int                    count,
                                                  const ComponentHandle* handles,
                                                  const FTransform*      targets,
                                                  int                    targetState,
                                                  float                  duration,
                                                  float                  delay)
{
        TransformComponent* transformComp = m_ControlledEntityHandle.GetComponent<TransformComponent>();

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
        TransformComponent* transformComp = m_ControlledEntityHandle.GetComponent<TransformComponent>();

        m_CinematicState->SetTransitionMode(E_TRANSITION_MODE::LookAt);
        m_CinematicState->AddTransformTransitions(count, handles, targets);
        m_CinematicState->SetLookAtTransitionDuration(lookAtTransitionDuration);
        m_CinematicState->SetLookAtTarget(lookAtTarget);
        m_CinematicState->SetTansitionTargetState(targetState);
        m_CinematicState->SetTansitionDuration(duration);
        m_CinematicState->SetTransitionDelay(delay);

        m_StateMachine.Transition(E_PLAYERSTATE_EVENT::TO_TRANSITION);
}

void PlayerController::RequestCurrentLevel()
{
        GEngine::Get()->GetLevelStateManager()->RequestState(0);
}

void PlayerController::RequestNextLevel()
{
        GEngine::Get()->GetLevelStateManager()->RequestNextLevel();
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
                TransformComponent* playerTransformComp = m_ControlledEntityHandle.GetComponent<TransformComponent>();
                GoalComponent*      goalComp            = goalHandle.Get<GoalComponent>();
                EntityHandle        goalCompParent      = goalComp->GetParent();
                ComponentHandle     goalTransformHandle = goalCompParent.GetComponentHandle<TransformComponent>();

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
