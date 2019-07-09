#include "PlayerControllerStateMachine.h"
#include "PlayerControllerState.h"

void PlayerControllerStateMachine::Update(float deltaTime, FTransform transform)
{
        IPlayerControllerState* pcState    = static_cast<IPlayerControllerState*>(m_Current);
        pcState->_cachedTransform       = transform;

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

void PlayerControllerStateMachine::Shutdown()
{
        __super::Shutdown();
}
