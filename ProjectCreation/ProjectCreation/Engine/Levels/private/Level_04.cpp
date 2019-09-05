#include <GEngine.h>
#include <Level_04.h>
#include <MathLibrary.h>

#include <ControllerSystem.h>

void Level_04::Enter()
{
        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_OrbitSystem->ClearTimedFunctions();
        if (static_cast<LevelStateMachine*>(stateMachine)->m_ForceLoad)
        {
                AudioManager::Get()->ResetMusic();

                controllerSys->ResetPlayer();

                m_OrbitSystem->InstantCreateOrbitSystem();


                m_OrbitSystem->ClearCollectedMask();
                m_OrbitSystem->collectedMask[E_LIGHT_ORBS::RED_LIGHTS]   = true;
                m_OrbitSystem->collectedMask[E_LIGHT_ORBS::GREEN_LIGHTS] = true;
                m_OrbitSystem->collectedMask[E_LIGHT_ORBS::BLUE_LIGHTS]  = true;

                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::RED_LIGHTS);
                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::GREEN_LIGHTS);
                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::BLUE_LIGHTS);

                GEngine::Get()->SetPlayerRadius(1500.0f);

                GEngine::Get()->m_TerrainAlpha = 1.0f;

                GEngine::Get()->ForceSetInstanceReveal(1.0f);

                GEngine::Get()->SetPuzzleState(0.0f);
        }

        else
        {
                AudioManager::Get()->PlaySoundWithVolume(1.0f, "TRANSITION_PLANTS");
        }

        m_SpeedBoostSystem->m_ColorsCollected[0] = false;
        m_SpeedBoostSystem->m_ColorsCollected[1] = false;
        m_SpeedBoostSystem->m_ColorsCollected[2] = false;
        m_SpeedBoostSystem->m_ColorsCollected[3] = true;
        GEngine::Get()->m_TargetInstanceReveal   = 1.0f;

        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        if (GEngine::Get()->GetCurrentPlayerRadius() == 0.0f)
        {
                GEngine::Get()->SetDesiredPlayerRadius(1500.0f);
                GEngine::Get()->SetTransitionSpeed(15.0f);
                GEngine::Get()->Update();
        }
        m_SpeedBoostSystem->ResetLevel();

        m_SpeedBoostSystem->splineWidth  = 18.0f;
        m_SpeedBoostSystem->splineHeight = 1.5f;

        controllerSys->ResetOrbCount();

        AudioManager::Get()->ActivateMusicAndPause(Waves, true);
        Waves->music->ResumeStream();

        AudioManager::Get()->ActivateMusicAndPause(Eruption, true);
        Eruption->music->ResumeStream();

        AudioManager::Get()->ActivateMusicAndPause(Forest, true);
        Forest->music->ResumeStream();

        m_SpeedBoostSystem->SetTargetTerrain(1.0f);

        m_LevelType = E_Level_States::LEVEL_04;
}

void Level_04::Update(float deltaTime)
{}

void Level_04::Exit()
{
        Waves->StopStream();
        Eruption->StopStream();
        Forest->StopStream();
}

Level_04::Level_04()
{
        m_OrbitSystem      = GEngine::Get()->GetSystemManager()->GetSystem<OrbitSystem>();
        m_LevelType        = E_Level_States::LEVEL_04;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_PlayerController              = static_cast<PlayerController*>(controllerSys->GetCurrentController());

        Waves = AudioManager::Get()->LoadMusic("Ambience_andWaves");
        Eruption = AudioManager::Get()->LoadMusic("EARTHQUAKE_SFX");
        Forest = AudioManager::Get()->LoadMusic("LEVEL_4_AMBIENCE");
}
