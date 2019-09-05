#include <Level_03.h>
#include <GEngine.h>
#include <MathLibrary.h>

#include <ControllerSystem.h>

void Level_03::Enter()
{
        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_OrbitSystem->ClearTimedFunctions();
        if (static_cast<LevelStateMachine*>(stateMachine)->m_ForceLoad)
        {
                AudioManager::Get()->ResetMusic();

                controllerSys->ResetPlayer();

                m_OrbitSystem->InstantCreateOrbitSystem();

                m_SpeedBoostSystem->m_ColorsCollected[0] = true;
                m_SpeedBoostSystem->m_ColorsCollected[1] = true;
                m_SpeedBoostSystem->m_ColorsCollected[2] = false;
                m_SpeedBoostSystem->m_ColorsCollected[3] = true;

                m_OrbitSystem->ClearCollectedMask();
                m_OrbitSystem->collectedMask[E_LIGHT_ORBS::RED_LIGHTS]   = true;
                m_OrbitSystem->collectedMask[E_LIGHT_ORBS::GREEN_LIGHTS] = true;
                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::GREEN_LIGHTS);
                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::RED_LIGHTS);
                GEngine::Get()->SetPlayerRadius(1500.0f);

                GEngine::Get()->m_TerrainAlpha = 1.0f;

                GEngine::Get()->SetPuzzleState(0.0f);
        }

		else
        {
                AudioManager::Get()->PlaySoundWithVolume(1.0f, "TRANSITION_LAND");
        }

        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        if (GEngine::Get()->GetCurrentPlayerRadius() == 0.0f)
        {
                GEngine::Get()->SetDesiredPlayerRadius(1500.0f);
                GEngine::Get()->SetTransitionSpeed(15.0f);
                GEngine::Get()->Update();
        }
        m_SpeedBoostSystem->ResetLevel();
        GEngine::Get()->ForceSetInstanceReveal(0.0f);

        m_SpeedBoostSystem->splineWidth  = 18.0f;
        m_SpeedBoostSystem->splineHeight = 1.5f;

        controllerSys->ResetOrbCount();

        AudioManager::Get()->ActivateMusicAndPause(Waves, true);
        Waves->music->ResumeStream();

        m_SpeedBoostSystem->SetTargetTerrain(1.0f);

        m_LevelType = E_Level_States::LEVEL_03;
	
}

void Level_03::Update(float deltaTime)
{
        // GEngine::Get()->m_TerrainAlpha = MathLibrary::lerp(GEngine::Get()->m_TerrainAlpha, 1.0f, deltaTime * 0.1f);
}

void Level_03::Exit()
{
        Waves->StopStream();
}

Level_03::Level_03()
{
        m_OrbitSystem      = GEngine::Get()->GetSystemManager()->GetSystem<OrbitSystem>();
        m_LevelType        = E_Level_States::LEVEL_03;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_PlayerController              = static_cast<PlayerController*>(controllerSys->GetCurrentController());

        Waves = AudioManager::Get()->LoadMusic("Ambience_andWaves");
}
