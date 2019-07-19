#pragma once
#include "LevelState.h"
#include "LevelStateEvents.h"
#include "LevelStateMachine.h"

class ILevelState;

class LevelStateManager
{
        friend class LevelStateMachine;

    private:
        LevelStateMachine m_LevelStateMachine;

        std::vector<ILevelState*> levelStates;

    public:
        inline ILevelState* GetCurrentLevelState()
        {
                return static_cast<ILevelState*>(m_LevelStateMachine.GetCurrentState());
        }

        void Init();
        void Shutdown();

        void RequestState(int level);
};