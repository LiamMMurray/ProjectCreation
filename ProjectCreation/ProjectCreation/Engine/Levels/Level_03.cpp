#include "Level_03.h"
#include "../GEngine.h"
#include "../MathLibrary/MathLibrary.h"

#include "../Controller/ControllerSystem.h"

void Level_03::Enter()
{

        m_SpeedBoostSystem->m_ColorsCollected[0] = false;
        m_SpeedBoostSystem->m_ColorsCollected[1] = false;
        m_SpeedBoostSystem->m_ColorsCollected[2] = false;
        m_SpeedBoostSystem->m_ColorsCollected[3] = true;


        if (m_PlayerController->GetCollectedPlanetCount() <= 0)
        {
                m_OrbitSystem->InstantCreateOrbitSystem();
                m_OrbitSystem->InstantRemoveFromOrbit();

                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::RED_LIGHTS);
                m_SpeedBoostSystem->m_ColorsCollected[0]                             = true;
                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->CollectOrbEventIDs[0] = 0;

                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::BLUE_LIGHTS);
                m_SpeedBoostSystem->m_ColorsCollected[2]                             = true;
                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->CollectOrbEventIDs[2] = 0;
        }

        else if (m_PlayerController->GetCollectedPlanetCount() <= 1 && m_PlayerController->GetCollectedPlanetCount() > 0)
        {
                m_OrbitSystem->InstantRemoveFromOrbit();

                double smallestTimestamp     = DBL_MAX;
                double nextSmallestTimestamp = DBL_MAX;

                int firstIndex  = -1;
                int secondIndex = -1;
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

                if (firstIndex == E_LIGHT_ORBS::BLUE_LIGHTS)
                {
                        m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::RED_LIGHTS);
                        m_SpeedBoostSystem->m_ColorsCollected[0]                             = true;
                        SYSTEM_MANAGER->GetSystem<ControllerSystem>()->CollectOrbEventIDs[0] = 0;
                }

                else
                {
                        m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::BLUE_LIGHTS);
                        m_SpeedBoostSystem->m_ColorsCollected[2]                             = true;
                        SYSTEM_MANAGER->GetSystem<ControllerSystem>()->CollectOrbEventIDs[2] = 0;
                }
        }

        else if (m_PlayerController->GetCollectedPlanetCount() > 2)
        {

                m_OrbitSystem->InstantRemoveFromOrbit();

                double smallestTimestamp     = DBL_MAX;
                double nextSmallestTimestamp = DBL_MAX;

                int firstIndex  = -1;
                int secondIndex = -1;

                for (int i = 0; i < 3; ++i)
                {
                        if (m_OrbitSystem->PlanetPickupTimestamps[i] < smallestTimestamp)
                        {
                                if (m_OrbitSystem->PlanetPickupTimestamps[i] >= 0)
                                {
                                        firstIndex = secondIndex = i;
                                }
                        }

                        else if (m_OrbitSystem->PlanetPickupTimestamps[i] < nextSmallestTimestamp)
                        {
                                if (m_OrbitSystem->PlanetPickupTimestamps[i] >= 0)
                                {
                                        secondIndex = i;
                                }
                        }
                }

                // Keep first planet collected
                m_OrbitSystem->InstantInOrbit(firstIndex);
                m_SpeedBoostSystem->m_ColorsCollected[firstIndex] = true;

                // Keep second planet collected
                m_OrbitSystem->InstantInOrbit(firstIndex);
                m_SpeedBoostSystem->m_ColorsCollected[firstIndex] = true;
        }

        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        if (GEngine::Get()->GetCurrentPlayerRadius() == 0.0f)
        {
                GEngine::Get()->SetDesiredPlayerRadius(1500.0f);
                GEngine::Get()->SetTransitionSpeed(15.0f);
                GEngine::Get()->Update();
        }
        m_SpeedBoostSystem->ResetLevel();
        GEngine::Get()->m_TargetInstanceReveal = 0.0f;

        m_SpeedBoostSystem->splineWidth  = 18.0f;
        m_SpeedBoostSystem->splineHeight = 1.5f;
        m_SpeedBoostSystem->changeColor  = true;

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        controllerSys->ResetLightOrbCounters();

        AudioManager::Get()->ActivateMusicAndPause(Waves, true);
        Waves->ResumeStream();

        m_SpeedBoostSystem->SetTargetTerrain(1.0f);
}

void Level_03::Update(float deltaTime)
{
        // GEngine::Get()->m_TerrainAlpha = MathLibrary::lerp(GEngine::Get()->m_TerrainAlpha, 1.0f, deltaTime * 0.1f);
}

void Level_03::Exit()
{}

Level_03::Level_03()
{
        m_OrbitSystem      = GEngine::Get()->GetSystemManager()->GetSystem<OrbitSystem>();
        m_LevelType        = E_Level_States::LEVEL_03;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_PlayerController              = static_cast<PlayerController*>(controllerSys->GetCurrentController());

        Waves = AudioManager::Get()->LoadMusic("Ambience_andWaves");
        Waves->SetVolume(0.6f);
}
