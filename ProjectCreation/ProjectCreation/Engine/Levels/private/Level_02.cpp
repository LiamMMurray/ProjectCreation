#include <Level_02.h>
#include "../../GEngine.h"

#include <ControllerSystem.h>

#include <limits>

void Level_02::Enter()
{
        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_OrbitSystem->ClearTimedFunctions();
        if (static_cast<LevelStateMachine*>(stateMachine)->m_ForceLoad)
        {
                controllerSys->ResetPlayer();

                m_OrbitSystem->InstantCreateOrbitSystem();

                m_SpeedBoostSystem->m_ColorsCollected[0] = true;
                m_SpeedBoostSystem->m_ColorsCollected[1] = false;
                m_SpeedBoostSystem->m_ColorsCollected[2] = false;
                m_SpeedBoostSystem->m_ColorsCollected[3] = true;

                m_OrbitSystem->ClearCollectedMask();
                m_OrbitSystem->collectedMask[E_LIGHT_ORBS::RED_LIGHTS] = true;
                m_OrbitSystem->InstantInOrbit(E_LIGHT_ORBS::RED_LIGHTS);

                GEngine::Get()->SetPlayerRadius(1500.0f);

                GEngine::Get()->SetPuzzleState(0.0f);
        }

		else
        {
                AudioManager::Get()->PlaySoundWithVolume(1.0f, "TRANSITION_WATER");
		}

        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        GEngine::Get()->SetDesiredPlayerRadius(1500.0f);
        GEngine::Get()->SetTransitionSpeed(15.0f);
        GEngine::Get()->Update();
        GEngine::Get()->ForceSetInstanceReveal(0.0f);

        m_SpeedBoostSystem->splineWidth  = 6.0f;
        m_SpeedBoostSystem->splineHeight = 0.7f;
        m_SpeedBoostSystem->ResetLevel();

        m_SpeedBoostSystem->SetTargetTerrain(0.0f);
        GEngine::Get()->m_TerrainAlpha = 0.0f;

        controllerSys->ResetOrbCount();

        AudioManager::Get()->ActivateMusicAndPause(Waves, true);
        Waves->ResumeStream();

        m_LevelType = E_Level_States::LEVEL_02;
}

void Level_02::Update(float deltaTime)
{}

void Level_02::Exit()
{
        //delete Waves;
}

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
