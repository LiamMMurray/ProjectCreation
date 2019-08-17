#include "Level_02.h"
#include "../GEngine.h"

#include "../Controller/ControllerSystem.h"


void Level_02::Enter()
{
        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        GEngine::Get()->SetDesiredPlayerRadius(1500.0f);
        GEngine::Get()->SetTransitionSpeed(15.0f);
        GEngine::Get()->Update();

        m_SpeedBoostSystem->splineWidth  = 6.0f;
        m_SpeedBoostSystem->splineHeight = 0.7f;
        m_SpeedBoostSystem->m_changeColor  = false;
        m_SpeedBoostSystem->ResetLevel();

        m_SpeedBoostSystem->SetTargetTerrain(0.0f);
        GEngine::Get()->m_TerrainAlpha = 0.0f;

		ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        controllerSys->ResetLightOrbCounters();

        m_LevelType = E_Level_States::LEVEL_02;
}

void Level_02::Update(float deltaTime)
{}

void Level_02::Exit()
{}

Level_02::Level_02()
{
        m_LevelType        = E_Level_States::LEVEL_02;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();
}
