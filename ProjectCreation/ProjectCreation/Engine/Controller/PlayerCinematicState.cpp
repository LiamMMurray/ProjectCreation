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
        m_currAlpha   = 0.0f;
        m_lookAtAlpha = 0.0f;

        m_LookAtTransitionDuration = min(m_LookAtTransitionDuration, m_Duration);

        size_t n = m_TransformComponents.size();
        m_InitTransforms.resize(n);
        for (size_t i = 0; i < n; ++i)
        {
                auto transformComp  = m_TransformComponents[i].Get<TransformComponent>();
                m_InitTransforms[i] = transformComp->transform;
        }

        EntityHandle        eHandle             = _playerController->GetControlledEntity();
        TransformComponent* playerTransformComp = eHandle.GetComponent<TransformComponent>();

        _playerInitialLookAtRot = playerTransformComp->transform.rotation;
}

void PlayerCinematicState::Update(float deltaTime)
{

        if (m_Delay > 0.0f)
        {
                m_Delay -= deltaTime;
                return;
        }
        m_currAlpha += deltaTime / m_Duration;
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
                case E_TRANSITION_MODE::Reveal:
                {
                        UpdateReveal(deltaTime);
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
        size_t n = m_TransformComponents.size();
        for (size_t i = 0; i < n; ++i)
        {
                FTransform          currentTransform;
                TransformComponent* transformComp = m_TransformComponents[i].Get<TransformComponent>();

                currentTransform = FTransform::Lerp(m_InitTransforms[i], m_EndTransforms[i], min(1.0f, m_currAlpha));

                transformComp->transform = currentTransform;
        }
}

void PlayerCinematicState::UpdateLookAt(float deltaTime)
{
        m_lookAtAlpha += deltaTime / m_LookAtTransitionDuration;

        size_t n = m_TransformComponents.size();
        for (size_t i = 0; i < n; ++i)
        {
                FTransform currentTransform;
                auto       transformComp = m_TransformComponents[i].Get<TransformComponent>();

                currentTransform = FTransform::Lerp(m_InitTransforms[i], m_EndTransforms[i], min(1.0f, m_currAlpha));

                transformComp->transform = currentTransform;
        }
        EntityHandle eHandle = _playerController->GetControlledEntity();

        TransformComponent* playerTransformComponent = eHandle.GetComponent<TransformComponent>();
        TransformComponent* lookAtTransformComponent = m_lookAtTarget.Get<TransformComponent>();

        FQuaternion desiredRotation = FQuaternion::LookAtWithRoll(playerTransformComponent->transform.translation,
                                                                  lookAtTransformComponent->transform.translation);

        playerTransformComponent->transform.rotation =
            FQuaternion::Lerp(_playerInitialLookAtRot, desiredRotation, min(1.0f, m_lookAtAlpha));
}

void PlayerCinematicState::UpdateReveal(float deltaTime)
{
        m_RevealRadius += 1.0f;
        GEngine::Get()->m_PlayerRadius = MathLibrary::lerp(GEngine::Get()->m_PlayerRadius, m_RevealRadius, deltaTime * 15.0f);
        if (GEngine::Get()->m_PlayerRadius >= 500)
        {
                _playerController->SetCollectedPlanetCount(0);
                m_currAlpha = 1.0f;
        }
}

void PlayerCinematicState::Exit()
{
        m_TransformComponents.clear();
        m_InitTransforms.clear();
        m_EndTransforms.clear();
}


void PlayerCinematicState::AddTransformTransitions(int count, const ComponentHandle* handles, const FTransform* ends)
{
        if (count == 0)
                return;

        m_TransformComponents.insert(m_TransformComponents.end(), handles, handles + count);
        m_EndTransforms.insert(m_EndTransforms.end(), ends, ends + count);
}

void PlayerCinematicState::SetTansitionDuration(float _duration)
{
        m_Duration = _duration;
}
