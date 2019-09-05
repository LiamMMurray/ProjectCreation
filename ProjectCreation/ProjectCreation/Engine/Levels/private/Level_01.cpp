#include <ControllerSystem.h>
#include <DirectXMath.h>
#include <Level_01.h>
#include <OrbitSystem.h>
#include <UIManager.h>
#include "LevelStateMachine.h"

using namespace DirectX;

void Level_01::Enter()
{
        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_OrbitSystem->ClearTimedFunctions();
        if (static_cast<LevelStateMachine*>(stateMachine)->m_ForceLoad)
        {
                AudioManager::Get()->ResetMusic();

                controllerSys->ResetPlayer();

                m_OrbitSystem->InstantCreateOrbitSystem();

                m_SpeedBoostSystem->m_ColorsCollected[0] = false;
                m_SpeedBoostSystem->m_ColorsCollected[1] = false;
                m_SpeedBoostSystem->m_ColorsCollected[2] = false;
                m_SpeedBoostSystem->m_ColorsCollected[3] = true;

                m_OrbitSystem->ClearCollectedMask();

                GEngine::Get()->SetPuzzleState(0.0f);
        }

        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        GEngine::Get()->SetPlayerRadius(0);

        m_SpeedBoostSystem->SetTargetTerrain(0.0f);
        GEngine::Get()->m_TerrainAlpha = 0.0f;

        GEngine::Get()->ForceSetInstanceReveal(0.0f);

        m_SpeedBoostSystem->splineWidth  = 3.0f;
        m_SpeedBoostSystem->splineHeight = 0.35f;
        m_SpeedBoostSystem->SetTargetTerrain(0.0f);

        m_SpeedBoostSystem->ResetLevel();

        controllerSys->ResetOrbCount();
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
