#include "Level_02.h"
#include "../GEngine.h"

#include "../Controller/ControllerSystem.h"

#include <limits>

void Level_02::Enter()
{
        if (m_PlayerController->GetCollectedPlanetCount() <= 0)
        {
                m_SpeedBoostSystem->m_ColorsCollected[0] = true;
                m_SpeedBoostSystem->m_ColorsCollected[1] = false;
                m_SpeedBoostSystem->m_ColorsCollected[2] = false;
                m_SpeedBoostSystem->m_ColorsCollected[3] = true;

                m_OrbitSystem->InstantCreateOrbitSystem();
                m_OrbitSystem->InstantRemoveFromOrbit();
                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::RED_LIGHTS);
                m_SpeedBoostSystem->m_ColorsCollected[0] = true;
                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->CollectOrbEventIDs[0] = 0;
        }

        else if (m_PlayerController->GetCollectedPlanetCount() > 1)
        {
                m_OrbitSystem->InstantRemoveFromOrbit();
                double smallestTimestamp = DBL_MAX;
                int    firstIndex        = -1;
                for (int i = 0; i < 3; ++i)
                {
                        if (m_OrbitSystem->PlanetPickupTimestamps[i] < smallestTimestamp &&
                            m_OrbitSystem->PlanetPickupTimestamps[i] >= 0)
                        {
                                firstIndex = i;
                        }
                }
                m_OrbitSystem->InstantInOrbit(firstIndex);
                m_SpeedBoostSystem->m_ColorsCollected[firstIndex] = true;
        }

        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        GEngine::Get()->SetDesiredPlayerRadius(1500.0f);
        GEngine::Get()->SetTransitionSpeed(15.0f);
        GEngine::Get()->Update();
        GEngine::Get()->m_TargetInstanceReveal = 0.0f;

        m_SpeedBoostSystem->splineWidth  = 6.0f;
        m_SpeedBoostSystem->splineHeight = 0.7f;
        m_SpeedBoostSystem->changeColor  = true;
        m_SpeedBoostSystem->ResetLevel();

        m_SpeedBoostSystem->SetTargetTerrain(0.0f);
        GEngine::Get()->m_TerrainAlpha = 0.0f;

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        controllerSys->ResetLightOrbCounters();

        AudioManager::Get()->ActivateMusicAndPause(Waves, true);
        Waves->ResumeStream();

        m_LevelType = E_Level_States::LEVEL_02;
}

void Level_02::Update(float deltaTime)
{}

void Level_02::Exit()
{}

Level_02::Level_02()
{
        m_OrbitSystem      = GEngine::Get()->GetSystemManager()->GetSystem<OrbitSystem>();
        m_LevelType        = E_Level_States::LEVEL_02;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_PlayerController              = static_cast<PlayerController*>(controllerSys->GetCurrentController());

        Waves = AudioManager::Get()->LoadMusic("Ambience_andWaves");
        Waves->SetVolume(0.6f);
}
