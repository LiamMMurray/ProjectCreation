#include "PlayerPuzzleState.h"
#include <CameraComponent.h>
#include "../../CollisionLibary/CollisionLibary.h"
#include "../../CoreInput/InputActions.h"
#include "../../GEngine.h"
#include "../../Gameplay/GoalComponent.h"
#include "../../Gameplay/OrbitSystem.h"
#include "../../GenericComponents/TransformComponent.h"
#include "../../MathLibrary/MathLibrary.h"
#include <PlayerMovement.h>
#include <PlayerStateEvents.h>

using namespace DirectX;

void PlayerPuzzleState::Enter()
{
        dragVelocity = XMVectorZero();

        auto goalComp                       = _playerController->GetGoalComponent().Get<GoalComponent>();
        goalComp->goalState                 = E_GOAL_STATE::Puzzle;
        GEngine::Get()->m_TargetPuzzleState = 1.0f;
}

void PlayerPuzzleState::Update(float deltaTime)
{
        auto     goalComp              = _playerController->GetGoalComponent().Get<GoalComponent>();
        auto     goalTransform         = goalComp->GetParent().GetComponent<TransformComponent>();
        auto     playerTransformHandle = _playerController->GetControlledEntity().GetComponentHandle<TransformComponent>();
        auto     playerTransformComp   = playerTransformHandle.Get<TransformComponent>();
        auto     cameraComponent       = _playerController->GetControlledEntity().GetComponent<CameraComponent>();
        XMVECTOR input                 = XMVectorZero();
        float    speed                 = MathLibrary::CalulateVectorLength(dragVelocity);

        float dot = MathLibrary::VectorDotProduct(input, dragVelocity);
        float accel;

        if (InputActions::CheckAction(goalComp->color) == KeyState::Down)
        {
                if (JGamePad::Get()->CheckConnection() == true)
                {
                        input = deltaTime *
                            XMVectorSet((float)JGamePad::Get()->leftStickX, (float)JGamePad::Get()->leftStickY, 0.0f, 0.0f) *
                                15.0f;
                }

                if (JGamePad::Get()->CheckConnection() == false)
                {
                        input = deltaTime *
                                XMVectorSet((float)GCoreInput::GetMouseX(), (float)-GCoreInput::GetMouseY(), 0.0f, 0.0f);
                }
        }
        else
        {}


        XMVECTOR offset = XMVectorZero();
        if (MathLibrary::CalulateVectorLength(input) > 0)
        {
                accel = dragAcceleration;
                if (dot < 0)
                        accel += dragAcceleration;
                dragVelocity += input * accel * deltaTime;
        }
        else
        {
                accel = dragDeacceleration;
                dragVelocity -= XMVector3Normalize(dragVelocity) * min(accel * deltaTime, speed);
        }
        dragVelocity = XMVector3ClampLength(dragVelocity, 0.0f, goalDragMaxSpeed);

        offset += XMVector3Rotate(dragVelocity, playerTransformComp->transform.rotation.data);
        XMVECTOR newPos = goalTransform->transform.translation + offset;
        bool     inBounds;
        XMVECTOR clampedPos;
        inBounds = CollisionLibary::PointInNDC(newPos, cameraComponent->_cachedViewProjection, clampedPos);


        if (inBounds)
        {
                goalTransform->transform.translation = newPos;
        }
        else
        {
                goalTransform->transform.translation = clampedPos;
        }
		//
        Shapes::FSphere sphereA(goalTransform->transform.translation, goalComp->initialTransform.GetRadius()*0.75f);
        Shapes::FSphere sphereB(goalComp->goalTransform.translation, goalComp->goalTransform.GetRadius()*0.75f);

        Collision::FOverlapResult result =
            CollisionLibary::ScreenSpaceOverlap(sphereA, sphereB, cameraComponent->_cachedViewProjection);

        if (result.hasOverlap)
        {
                // stateMachine->Transition();
                auto currGoalComp = goalComp->GetParent().GetComponent<TransformComponent>();

                FTransform targetTransform;
                targetTransform.translation = XMVectorSetY(playerTransformComp->transform.translation, 0.0f);
                targetTransform.rotation    = FQuaternion::FromEulerAngles(0.0f, 0.0f, 0.0f);
                _playerController->RequestCinematicTransition(
                    1, &playerTransformHandle, &targetTransform, E_PLAYERSTATE_EVENT::TO_GROUND, 4.0f, 1.0f);

                goalComp->initialTransform = goalTransform->transform;
                goalComp->goalState        = E_GOAL_STATE::Done;
                goalComp->goalState        = E_GOAL_STATE::Done;

                auto orbitSystem                      = SYSTEM_MANAGER->GetSystem<OrbitSystem>();
                orbitSystem->activeGoal.hasActiveGoal = false;
                orbitSystem->goalsCollected++;
                orbitSystem->collectedMask[orbitSystem->activeGoal.activeColor] = true;

                AudioManager::Get()->PlaySoundWithVolume(0.6f, "PUZZLE_COMPLETION");
        }
}

void PlayerPuzzleState::Exit()
{
        GEngine::Get()->m_TargetPuzzleState = 0.0f;
}
