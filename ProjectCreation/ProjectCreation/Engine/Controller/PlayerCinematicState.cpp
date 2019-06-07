#include "PlayerCinematicState.h"

#include "..//GEngine.h"
#include "..//Gameplay/GoalComponent.h"
#include "..//GenericComponents/TransformComponent.h"
#include "..//MathLibrary/MathLibrary.h"
#include "../CoreInput/CoreInput.h"
#include "PlayerControllerStateMachine.h"
#include "PlayerMovement.h"

void PlayerCinematicState::Enter()
{}

void PlayerCinematicState::Update(float deltaTime)
{
        GoalComponent* goalComp =
            GEngine::Get()->GetComponentManager()->GetComponent<GoalComponent>(_playerController->GetGoalComponent());
        TransformComponent* goalTransform =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(goalComp->GetOwner());

		//goalTran
}

void PlayerCinematicState::Exit()
{}
