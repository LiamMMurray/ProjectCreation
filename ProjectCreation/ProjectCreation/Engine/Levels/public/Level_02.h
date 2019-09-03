#pragma once
#include "../../Audio/AudioManager.h"
#include "../../Gameplay/OrbitSystem.h"
#include "../../Gameplay/SpeedBoostSystem.h"
#include <PlayerMovement.h>
#include <LevelState.h>

// LEVEL 2 : OCEAN
class Level_02 : public ILevelState
{
        SpeedBoostSystem*  m_SpeedBoostSystem;
        OrbitSystem*       m_OrbitSystem;
        PlayerController*  m_PlayerController;
        GW::AUDIO::GMusic* Waves;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_02();
        virtual ~Level_02() = default;
};