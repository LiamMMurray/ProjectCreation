#pragma once
#include "../../Gameplay/StateMachine/IState.h"

// make enum

enum E_Level_States
{
        TUTORIAL_LEVEL = 0,
        LEVEL_01,
        LEVEL_02,
        LEVEL_03,
        LEVEL_04,
        CREDITS_LEVEL,
        COUNT
};

class ILevelState : public IState
{
        friend class LevelStateMachine;

    protected:
        int  m_LevelType = E_Level_States::TUTORIAL_LEVEL;

        virtual void Enter()                 = 0;
        virtual void Update(float deltaTime) = 0;
        virtual void Exit()                  = 0;

    public:
        inline int GetLevelType() const
        {
                return m_LevelType;
        }
};