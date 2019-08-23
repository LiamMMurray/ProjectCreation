#include "Level_01.h"
#include "../Controller/ControllerSystem.h"
#include "../Gameplay/OrbitSystem.h"

#include <DirectXMath.h>

using namespace DirectX;

void Level_01::Enter()
{
        //m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->sunHandle);
        //m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring1Handle);
        //m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring2Handle);
        //m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);	

		m_OrbitSystem->InstantCreateOrbitSystem();
        m_OrbitSystem->InstantRemoveFromOrbit();

        m_SpeedBoostSystem->m_ColorsCollected[0] = false;
        m_SpeedBoostSystem->m_ColorsCollected[1] = false;
        m_SpeedBoostSystem->m_ColorsCollected[2] = false;
        m_SpeedBoostSystem->m_ColorsCollected[3] = true;


        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        GEngine::Get()->SetPlayerRadius(0);
        GEngine::Get()->m_TerrainAlpha   = 0.0f;
        GEngine::Get()->m_TargetInstanceReveal = 0.0f;

		m_SpeedBoostSystem->splineWidth  = 3.0f;
        m_SpeedBoostSystem->splineHeight = 0.35f;
        m_SpeedBoostSystem->changeColor  = false;
        m_SpeedBoostSystem->inTutorial   = false;
        m_SpeedBoostSystem->SetTargetTerrain(0.0f);

        m_SpeedBoostSystem->ResetLevel();

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        controllerSys->ResetLightOrbCounters();
}

void Level_01::Update(float deltaTime)
{}

void Level_01::Exit()
{}

Level_01::Level_01()
{
        m_OrbitSystem      = GEngine::Get()->GetSystemManager()->GetSystem<OrbitSystem>();
        m_LevelType        = E_Level_States::LEVEL_01;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();
}
