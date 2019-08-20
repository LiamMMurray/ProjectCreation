#pragma once
#include "LevelState.h"
#include "../Gameplay/SpeedBoostSystem.h"
#include "../Gameplay/OrbitSystem.h"

// LEVEL 2 : OCEAN
class Level_02 : public ILevelState
{
        SpeedBoostSystem* m_SpeedBoostSystem;
        OrbitSystem*      m_OrbitSystem;
    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_02();
        virtual ~Level_02() = default;
};