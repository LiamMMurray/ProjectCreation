#include "LevelStateManager.h"

#include "Level_01.h"
#include "Level_02.h"
#include "TutorialLevel.h"

void LevelStateManager::Init()
{
        levelStates.resize(E_Level_States::COUNT);

        levelStates[0] = m_LevelStateMachine.CreateState<TutorialLevel>();
        levelStates[1] = m_LevelStateMachine.CreateState<Level_01>();
        levelStates[2] = m_LevelStateMachine.CreateState<Level_02>();

        m_LevelStateMachine.SetCurrentState(static_cast<IState*>(levelStates[0]));

        m_LevelStateMachine.AddTransition(levelStates[0], levelStates[0], E_LevelStateEvents::TO_TUTORIAL_LEVEL);
        m_LevelStateMachine.AddTransition(levelStates[0], levelStates[1], E_LevelStateEvents::TO_LEVEL_01);
        m_LevelStateMachine.AddTransition(levelStates[1], levelStates[2], E_LevelStateEvents::TO_LEVEL_02);

        // RequestState(0);
}

void LevelStateManager::Shutdown()
{
        m_LevelStateMachine.Shutdown();
}

void LevelStateManager::Update(float deltaTime)
{
        static_cast<TutorialLevel*>(levelStates[0])->Update(deltaTime);
}

void LevelStateManager::RequestState(int level)
{
        m_LevelStateMachine.Transition(level);
}

void LevelStateManager::RequestNextLevel()
{
        ILevelState* levelState = static_cast<ILevelState*>(m_LevelStateMachine.GetCurrentState());
        int          currLevel  = levelState->GetLevelType();
        RequestState(currLevel + 1);
}
