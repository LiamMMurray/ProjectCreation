#include <StateMachine.h>

void StateMachine::Update(float deltaTime)
{
        if (m_Current)
                m_Current->Update(deltaTime);
}

void StateMachine::Shutdown()
{
        for (auto& state : m_States)
        {
                delete state;
        }

        m_States.clear();
        m_TransitionTable.clear();
        m_Current = nullptr;
}

void StateMachine::Transition(int event)
{
        auto it = m_TransitionTable.find(std::make_pair(m_Current, event));
        if (it != m_TransitionTable.end())
        {
                m_Current->Exit();
                m_Current = it->second;
                m_Current->Enter();
        }
}
