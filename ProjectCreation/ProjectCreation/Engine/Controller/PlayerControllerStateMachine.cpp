#include "PlayerControllerStateMachine.h"
#include "PlayerControllerState.h"

void PlayerControllerStateMachine::Update(float deltaTime, TransformComponent* transformComp)
{
        IPlayerControllerState* pcState    = static_cast<IPlayerControllerState*>(m_Current);
        pcState->_cachedTransformComponent = transformComp;

        __super::Update(deltaTime);
}

void PlayerControllerStateMachine::Init(PlayerController* playerMovement)
{
        for (auto& state : m_States)
        {
                ((IPlayerControllerState*)(state))->_playerController = playerMovement;
        }

		m_Current = m_States.front();
        m_Current->Enter();
}
