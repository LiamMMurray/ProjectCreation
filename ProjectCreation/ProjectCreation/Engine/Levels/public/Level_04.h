#pragma once
#include <AudioManager.h>
#include <PlayerMovement.h>
#include <OrbitSystem.h>
#include <SpeedBoostSystem.h>
#include <LevelState.h>

// LEVEL 4 : PLANTS
class Level_04 : public ILevelState
{
        friend class SpeedBoostSystem;

        SpeedBoostSystem*  m_SpeedBoostSystem;
        OrbitSystem*       m_OrbitSystem;
        PlayerController*  m_PlayerController;

        FMusic* Waves;
        FMusic* Eruption;
        FMusic* Forest;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_04();
        virtual ~Level_04() = default;
};