#include "LevelStateManager.h"

#include "Level_01.h"
#include "Level_02.h"
#include "Level_03.h"
#include "TutorialLevel.h"

void LevelStateManager::Init()
{
        levelStates.resize(E_Level_States::COUNT);

        levelStates[0] = m_LevelStateMachine.CreateState<TutorialLevel>();
        levelStates[1] = m_LevelStateMachine.CreateState<Level_01>();
        levelStates[2] = m_LevelStateMachine.CreateState<Level_02>();
        levelStates[3] = m_LevelStateMachine.CreateState<Level_03>();

        m_LevelStateMachine.SetCurrentState(static_cast<IState*>(levelStates[0]));
		
	// Game flow level transitions
        m_LevelStateMachine.AddTransition(levelStates[0], levelStates[0], E_LevelStateEvents::TUTORIAL_LEVEL_TO_TUTORIAL_LEVEL);
        m_LevelStateMachine.AddTransition(levelStates[1], levelStates[1], E_LevelStateEvents::LEVEL_01_TO_LEVEL_01);
        m_LevelStateMachine.AddTransition(levelStates[2], levelStates[2], E_LevelStateEvents::LEVEL_02_TO_LEVEL_02);
        m_LevelStateMachine.AddTransition(levelStates[3], levelStates[3], E_LevelStateEvents::LEVEL_03_TO_LEVEL_03);

	// Button tutorial level transitions
        m_LevelStateMachine.AddTransition(levelStates[0], levelStates[1], E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_01);
        m_LevelStateMachine.AddTransition(levelStates[0], levelStates[2], E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_02);
        m_LevelStateMachine.AddTransition(levelStates[0], levelStates[3], E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_03);
		
	// Button level 1 transitions
        m_LevelStateMachine.AddTransition(levelStates[1], levelStates[0], E_LevelStateEvents::LEVEL_01_TO_TUTORIAL_LEVEL);
        m_LevelStateMachine.AddTransition(levelStates[1], levelStates[2], E_LevelStateEvents::LEVEL_01_TO_LEVEL_02);
        m_LevelStateMachine.AddTransition(levelStates[1], levelStates[3], E_LevelStateEvents::LEVEL_01_TO_LEVEL_03);

	// Button level 2 transitions
        m_LevelStateMachine.AddTransition(levelStates[2], levelStates[0], E_LevelStateEvents::LEVEL_02_TO_TUTORIAL_LEVEL);
        m_LevelStateMachine.AddTransition(levelStates[2], levelStates[2], E_LevelStateEvents::LEVEL_02_TO_LEVEL_01);
        m_LevelStateMachine.AddTransition(levelStates[2], levelStates[3], E_LevelStateEvents::LEVEL_02_TO_LEVEL_03);

	// Button level 3 transitions
        m_LevelStateMachine.AddTransition(levelStates[3], levelStates[0], E_LevelStateEvents::LEVEL_03_TO_TUTORIAL_LEVEL);
        m_LevelStateMachine.AddTransition(levelStates[3], levelStates[1], E_LevelStateEvents::LEVEL_03_TO_LEVEL_01);
        m_LevelStateMachine.AddTransition(levelStates[3], levelStates[2], E_LevelStateEvents::LEVEL_03_TO_LEVEL_02);

        // RequestState(0);
}

void LevelStateManager::Shutdown()
{
        m_LevelStateMachine.Shutdown();
}

void LevelStateManager::Update(float deltaTime)
{
        if (m_LevelStateMachine.GetCurrentState() == levelStates[0])
        {
                static_cast<TutorialLevel*>(levelStates[0])->Update(deltaTime);
        }
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
