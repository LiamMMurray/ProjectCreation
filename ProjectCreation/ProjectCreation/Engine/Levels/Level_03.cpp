#include "Level_03.h"
#include "../GEngine.h"
#include "../MathLibrary/MathLibrary.h"

#include "../Controller/ControllerSystem.h"

void Level_03::Enter()
{
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->sunHandle);
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring1Handle);
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring2Handle);
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);

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
}
