#pragma once
#include <AudioManager.h>
#include <OrbitSystem.h>
#include <SpeedBoostSystem.h>
#include <PlayerMovement.h>
#include <LevelState.h>

// LEVEL 3 : LAND
class Level_03 : public ILevelState
{
        friend class SpeedBoostSystem;

        SpeedBoostSystem*  m_SpeedBoostSystem;
        OrbitSystem*       m_OrbitSystem;
        PlayerController*  m_PlayerController;
        GW::AUDIO::GMusic* Waves;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_03();
        virtual ~Level_03() = default;
};
