#pragma once
#include "LevelState.h"

class Level_02 : public ILevelState
{
    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_02();
        virtual ~Level_02() = default;
};