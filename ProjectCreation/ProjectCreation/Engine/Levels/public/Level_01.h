#pragma once
#include <LevelState.h>
#include "../../GEngine.h"
#include <SpeedBoostSystem.h>
#include <OrbitSystem.h>


// LEVEL 1: SPACE
class Level_01 : public ILevelState
{
        friend class SpeedBoostSystem;

        SpeedBoostSystem* m_SpeedBoostSystem;
        OrbitSystem*      m_OrbitSystem;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_01();
        virtual ~Level_01() = default;
};