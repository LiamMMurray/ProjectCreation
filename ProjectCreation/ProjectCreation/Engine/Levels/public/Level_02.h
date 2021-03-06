#pragma once
#include <AudioManager.h>
#include <OrbitSystem.h>
#include <SpeedBoostSystem.h>
#include <PlayerMovement.h>
#include <LevelState.h>

// LEVEL 2 : OCEAN
class Level_02 : public ILevelState
{
        SpeedBoostSystem*  m_SpeedBoostSystem;
        OrbitSystem*       m_OrbitSystem;
        PlayerController*  m_PlayerController;
        FMusic* Waves;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_02();
        virtual ~Level_02() = default;
};