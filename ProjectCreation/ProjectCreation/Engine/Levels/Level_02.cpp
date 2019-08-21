#include "Level_02.h"
#include "../GEngine.h"

#include "../Controller/ControllerSystem.h"

void Level_02::Enter()
{
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->sunHandle);
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring1Handle);
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring2Handle);
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);

        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        GEngine::Get()->SetDesiredPlayerRadius(1500.0f);
        GEngine::Get()->SetTransitionSpeed(15.0f);
        GEngine::Get()->Update();
        GEngine::Get()->m_TargetInstanceReveal = 0.0f;

        m_SpeedBoostSystem->splineWidth  = 6.0f;
        m_SpeedBoostSystem->splineHeight = 0.7f;
        m_SpeedBoostSystem->changeColor  = false;
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
        Waves         = AudioManager::Get()->LoadMusic("Ambience_andWaves");
        Waves->SetVolume(0.6f);
}
