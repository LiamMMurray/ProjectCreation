#pragma once
#include "LevelState.h"

class Level_01 : public ILevelState
{
    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;
        virtual ~Level_01() = default;
};