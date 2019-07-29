#pragma once
#include "LevelState.h"
#include "../Gameplay/SpeedBoostSystem.h"

// LEVEL 3 : LAND
class Level_03 : public ILevelState
{
        friend class SpeedBoostSystem;

        SpeedBoostSystem* m_SpeedBoostSystem;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

		Level_03();
        virtual ~Level_03() = default;
};
