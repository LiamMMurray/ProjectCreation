#include "PlayerEndState.h"
#include <PlayerMovement.h>
#include <UIManager.h>

void PlayerEndState::Enter()
{
        m_Timer     = 10.0f;
        m_GameEnded = false;
}

void PlayerEndState::Update(float deltaTime)
{
        if (m_Timer > 0.0f)
        {
                using namespace DirectX;

                m_Timer -= deltaTime;
                _cachedTransform.translation += VectorConstants::Up * deltaTime * 25.0f;
                _playerController->GetControlledEntity().GetComponent<TransformComponent>()->transform.translation =
                    _cachedTransform.translation;
        }
        else if (m_GameEnded == false)
        {
                UIManager::instance->DemoEnd();
                m_GameEnded = true;
        }
}

void PlayerEndState::Exit()
{}
