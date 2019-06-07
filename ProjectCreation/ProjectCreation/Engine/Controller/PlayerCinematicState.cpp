#include "PlayerCinematicState.h"

#include "..//GEngine.h"
#include "..//Gameplay/GoalComponent.h"
#include "..//GenericComponents/TransformComponent.h"
#include "..//MathLibrary/MathLibrary.h"
#include "../CoreInput/CoreInput.h"
#include "PlayerControllerStateMachine.h"
#include "PlayerMovement.h"

using namespace DirectX;

void PlayerCinematicState::Enter()
{
        m_currAlpha      = 0.0f;
        m_cinematicAlpha = 0.0f;
}

void PlayerCinematicState::Update(float deltaTime)
{
        FTransform currentTransform;
        m_cinematicAlpha += deltaTime;

        if (m_cinematicAlpha > m_Delay)
        {
                m_currAlpha += deltaTime / m_Duration;

                auto playerTransformComponent = GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(
                    _playerController->GetControlledEntity());

                XMVECTOR scale = playerTransformComponent->transform.scale;

                currentTransform.translation =
                    XMVectorLerp(m_initTransform.translation, m_endTransform.translation, m_currAlpha);
                currentTransform.rotation =
                    FQuaternion::Lerp(m_initTransform.rotation, m_endTransform.translation, m_currAlpha);

                playerTransformComponent->transform = currentTransform;

                playerTransformComponent->transform.scale = scale;

                if (m_currAlpha >= 1.0f)
                {
                        stateMachine->Transition(E_STATE_EVENT::EASE_IN_FINISH);
                }
        }
}

void PlayerCinematicState::Exit()
{}

void PlayerCinematicState::SetInitTransform(FTransform _transform)
{
        m_initTransform = _transform;
}

void PlayerCinematicState::SetEndTransform(FTransform _transform)
{
        m_endTransform = _transform;
}

void PlayerCinematicState::SetTansitionDuration(float _duration)
{
        m_Duration = _duration;
}
