#pragma once

#include <ECSTypes.h>
#include "..//Gameplay/StateMachine/StateMachine.h"

class LevelStateMachine : public StateMachine
{

    public:
        bool m_ForceLoad = false;
        void         Update(float deltaTime);
        void         Init();
        virtual void Shutdown() override;
};