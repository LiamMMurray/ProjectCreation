#include "PlayerPuzzleState.h"
#include "..//..//Rendering/Components/CameraComponent.h"
#include "..//CollisionLibary/CollisionLibary.h"
#include "..//CoreInput/CoreInput.h"
#include "..//GEngine.h"
#include "..//Gameplay/GoalComponent.h"
#include "..//Gameplay/OrbitSystem.h"
#include "..//GenericComponents/TransformComponent.h"
#include "..//MathLibrary/MathLibrary.h"
#include "PlayerMovement.h"
#include "PlayerStateEvents.h"

using namespace DirectX;

void PlayerPuzzleState::Enter()
{
        dragVelocity = XMVectorZero();

        auto goalComp       = _playerController->GetGoalComponent().Get<GoalComponent>();
        goalComp->goalState = E_GOAL_STATE::Puzzle;
}

void PlayerPuzzleState::Update(float deltaTime)
{
        auto goalComp              = _playerController->GetGoalComponent().Get<GoalComponent>();
        auto goalTransform         = goalComp->GetParent().GetComponent<TransformComponent>();
        auto playerTransformHandle = _playerController->GetControlledEntity().GetComponentHandle<TransformComponent>();
        auto playerTransformComp   = playerTransformHandle.Get<TransformComponent>();
        auto cameraComponent       = _playerController->GetControlledEntity().GetComponent<CameraComponent>();
        XMVECTOR input = deltaTime * XMVectorSet((float)GCoreInput::GetMouseX(), (float)-GCoreInput::GetMouseY(), 0.0f, 0.0f);
        float    speed = MathLibrary::CalulateVectorLength(dragVelocity);

        float dot = MathLibrary::VectorDotProduct(input, dragVelocity);
        float accel;

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
                dragVelocity -= XMVector3Normalize(dragVelocity) * std::min(accel * deltaTime, speed);
        }
        dragVelocity = XMVector3ClampLength(dragVelocity, 0.0f, goalDragMaxSpeed);

        offset += XMVector3Rotate(dragVelocity, playerTransformComp->transform.rotation.data);
        goalTransform->transform.translation += offset;

        Shapes::FSphere sphereA(goalTransform->transform.translation, goalComp->initialTransform.GetRadius());
        Shapes::FSphere sphereB(goalComp->goalTransform.translation, goalComp->goalTransform.GetRadius());

        Collision::FOverlapResult result =
            CollisionLibary::ScreenSpaceOverlap(sphereA, sphereB, cameraComponent->_cachedViewProjection);

        if (result.hasOverlap)
        {
                // stateMachine->Transition();
                auto tHandle      = GEngine::Get()->GetSystemManager()->GetSystem<OrbitSystem>()->GetClosestGoalTransform();
                auto tComp        = tHandle.Get<TransformComponent>();
                auto currGoalComp = goalComp->GetParent().GetComponent<TransformComponent>();

                if (tComp == playerTransformComp || tComp == currGoalComp)
                {
                        FTransform targetTransform;
                        targetTransform.translation = XMVectorSetY(tComp->transform.translation, 0.0f);
                        targetTransform.rotation    = FQuaternion::FromEulerAngles(0.0f, 0.0f, 0.0f);
                        _playerController->RequestCinematicTransition(
                            1, &playerTransformHandle, &targetTransform, E_PLAYERSTATE_EVENT::TO_GROUND, 4.0f, 1.0f);
                }
                else
                {
                        XMVECTOR targetPos = playerTransformComp->transform.translation;
                        targetPos          = XMVectorSetY(targetPos, 0.0f);

                        _playerController->RequestCinematicTransitionLookAt(
                            tHandle, 0, nullptr, nullptr, E_PLAYERSTATE_EVENT::TO_GROUND, 4.0f, 4.0f, 1.0f);
                        m_CollectedPlanetCount++;
                        _playerController->SetCollectedPlanetCount(m_CollectedPlanetCount);
                }
                goalComp->initialTransform = goalTransform->transform;
                goalComp->goalState        = E_GOAL_STATE::Done;
        }
}

void PlayerPuzzleState::Exit()
{}
