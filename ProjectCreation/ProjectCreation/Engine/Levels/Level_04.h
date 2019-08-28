#pragma once
#include "../Audio/AudioManager.h"
#include "../Controller/PlayerMovement.h"
#include "../Gameplay/OrbitSystem.h"
#include "../Gameplay/SpeedBoostSystem.h"
#include "LevelState.h"

// LEVEL 4 : PLANTS
class Level_04 : public ILevelState
{
        friend class SpeedBoostSystem;

        SpeedBoostSystem*  m_SpeedBoostSystem;
        OrbitSystem*       m_OrbitSystem;
        PlayerController*  m_PlayerController;

        GW::AUDIO::GMusic* Waves;
        GW::AUDIO::GMusic* Eruption;
        GW::AUDIO::GMusic* Forest;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_04();
        virtual ~Level_04() = default;
};
