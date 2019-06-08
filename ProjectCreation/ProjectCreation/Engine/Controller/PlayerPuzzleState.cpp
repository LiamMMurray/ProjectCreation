#include "PlayerPuzzleState.h"
#include "..//CoreInput/CoreInput.h"
#include "..//GEngine.h"
#include "..//Gameplay/GoalComponent.h"
#include "..//GenericComponents/TransformComponent.h"
#include "..//MathLibrary/MathLibrary.h"
#include "PlayerMovement.h"

using namespace DirectX;

void PlayerPuzzleState::Enter()
{
        dragVelocity = XMVectorZero();

        auto goalComp =
            GEngine::Get()->GetComponentManager()->GetComponent<GoalComponent>(_playerController->GetGoalComponent());
        goalComp->goalState = E_GOAL_STATE::Puzzle;
}

void PlayerPuzzleState::Update(float deltaTime)
{
        auto goalComp =
            GEngine::Get()->GetComponentManager()->GetComponent<GoalComponent>(_playerController->GetGoalComponent());
        auto playerTransformComp =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(_playerController->GetControlledEntity());

        XMVECTOR input = deltaTime * XMVectorSet(GCoreInput::GetMouseX(), -GCoreInput::GetMouseY(), 0.0f, 0.0f);
        float    speed = MathLibrary::CalulateVectorLength(dragVelocity);

        float dot   = MathLibrary::VectorDotProduct(input, dragVelocity);
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

        goalComp->initialTransform.translation += offset;
}

void PlayerPuzzleState::Exit()
{}
