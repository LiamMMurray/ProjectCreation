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
        m_currAlpha = 0.0f;
}

void PlayerCinematicState::Update(float deltaTime)
{

        if (m_Delay > 0.0f)
        {
                m_Delay -= deltaTime;
                return;
        }

        switch (m_transitionMode)
        {
                case E_TRANSITION_MODE::Simple:
                {
                        UpdateSimple(deltaTime);
                        break;
                }

                case E_TRANSITION_MODE::LookAt:
                {
                        UpdateLookAt(deltaTime);
                        break;
                }
                default:
                {
                        assert(false && "invalid transition mode requested");
                }
        }

        if (m_currAlpha >= 1.0f)
        {
                stateMachine->Transition(m_TargetState);
        }
}

void PlayerCinematicState::UpdateSimple(float deltaTime)
{
        FTransform currentTransform;

        m_currAlpha += deltaTime / m_Duration;

        auto playerTransformComponent =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(_playerController->GetControlledEntity());

        XMVECTOR scale = playerTransformComponent->transform.scale;

        currentTransform.translation =
            XMVectorLerp(m_initTransform.translation, m_endTransform.translation, std::min(1.0f, m_currAlpha));
        currentTransform.rotation =
            FQuaternion::Lerp(m_initTransform.rotation, m_endTransform.rotation, std::min(1.0f, m_currAlpha));

        playerTransformComponent->transform       = currentTransform;
        playerTransformComponent->transform.scale = scale;
}

void PlayerCinematicState::UpdateLookAt(float deltaTime)
{
        FTransform currentTransform;

        m_currAlpha += deltaTime / m_Duration;

        auto playerTransformComponent =
            GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(_playerController->GetControlledEntity());

        auto lookAtTransformComponent = GEngine::Get()->GetComponentManager()->GetComponent<TransformComponent>(m_lookAtTarget);

        XMVECTOR scale = playerTransformComponent->transform.scale;

        currentTransform.translation =
            XMVectorLerp(m_initTransform.translation, m_endTransform.translation, std::min(1.0f, m_currAlpha));
        currentTransform.rotation =
            FQuaternion::LookAtWithRoll(currentTransform.translation, lookAtTransformComponent->transform.translation);

        playerTransformComponent->transform       = currentTransform;
        playerTransformComponent->transform.scale = scale;
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
