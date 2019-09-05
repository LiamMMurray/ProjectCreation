#pragma once
#include <AudioManager.h>
#include <LevelState.h>
#include <OrbitSystem.h>
#include <PlayerMovement.h>
#include <SpeedBoostSystem.h>

// LEVEL 3 : LAND
class Level_03 : public ILevelState
{
        friend class SpeedBoostSystem;

        SpeedBoostSystem* m_SpeedBoostSystem;
        OrbitSystem*      m_OrbitSystem;
        PlayerController* m_PlayerController;
        FMusic*           Waves;

    public:
        // Inherited via ILevelState
        void Enter() override;
        void Update(float deltaTime) override;
        void Exit() override;

        Level_03();
        virtual ~Level_03() = default;
};
