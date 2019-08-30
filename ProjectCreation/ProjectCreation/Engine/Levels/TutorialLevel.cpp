#include "TutorialLevel.h"
#include "../../UI/UIManager.h"
#include "../Controller/ControllerSystem.h"

using namespace DirectX;

class SpeedBoostSystem;
class SpeedBoostComponent;

TutorialLevel*                  TutorialLevel::Instance  = nullptr;
TutorialLevel::E_TUTORIAL_PHASE TutorialLevel::currPhase = E_TUTORIAL_PHASE::PHASE_1;

void TutorialLevel::Enter()
{
        m_SpeedBoostSystem->SetRandomSpawnEnabled(false);

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        if (static_cast<LevelStateMachine*>(stateMachine)->m_ForceLoad)
        {
                controllerSys->ResetPlayer();
                GEngine::Get()->SetPuzzleState(0.0f);
        }

        m_OrbitSystem->ClearTimedFunctions();
        m_OrbitSystem->InstantRemoveOrbitSystem();
        // Update(GEngine::Get()->GetDeltaTime());
        Instance       = this;

        m_SpeedBoostSystem->m_ColorsCollected[0] = false;
        m_SpeedBoostSystem->m_ColorsCollected[1] = false;
        m_SpeedBoostSystem->m_ColorsCollected[2] = false;
        m_SpeedBoostSystem->m_ColorsCollected[3] = false;

        currPhase = phases[currPhaseIndex];

        GEngine::Get()->SetPlayerRadius(0);

        m_SpeedBoostSystem->splineWidth  = 1.0f;
        m_SpeedBoostSystem->splineHeight = 0.25f;

        m_SpeedBoostSystem->ResetLevel();

        m_OrbitSystem->goalsCollected = 0;

        m_WhiteCollected = m_RedCollected = m_GreenCollected = m_BlueCollected = false;
        whiteCount = redCount = greenCount = blueCount = levelRequested = 0;

        controllerSys->ResetOrbCount();
        GEngine::Get()->ForceSetInstanceReveal(0.0f);

        m_SpeedBoostSystem->SetTargetTerrain(0.0f);
        GEngine::Get()->m_TerrainAlpha = 0.0f;
        m_OrbitSystem->ClearCollectedMask();

        m_LevelType = E_Level_States::TUTORIAL_LEVEL;

        UpdatePhase1(0.0f);
}

void TutorialLevel::Update(float deltaTime)
{
        //switch (currPhase)
        //{
        //        case E_TUTORIAL_PHASE::PHASE_2:
        //        {
        //                m_SpeedBoostSystem->m_ColorsCollected[0] = false;
        //                for (int i = 1; i < 4; ++i)
        //                {
        //                        m_SpeedBoostSystem->m_ColorsCollected[i] = true;
        //                }
        //        }
        //}
}


void TutorialLevel::Exit()
{}

TutorialLevel::TutorialLevel()
{

        m_OrbitSystem      = GEngine::Get()->GetSystemManager()->GetSystem<OrbitSystem>();
        m_LevelType        = E_Level_States::TUTORIAL_LEVEL;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();
        whiteCount = redCount = blueCount = greenCount = levelRequested = 0;

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        m_PlayerController              = static_cast<PlayerController*>(controllerSys->GetCurrentController());
        m_PlayerEntityHandle = controllerSys->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]->GetControlledEntity();
        // m_PlayerController = m_PlayerEntityHandle.GetComponent<PlayerController>();

        m_PlayerTransform = m_PlayerEntityHandle.GetComponent<TransformComponent>();

        m_HandleManager = GEngine::Get()->GetHandleManager();
        currPhaseIndex  = 0;
}

void TutorialLevel::RequestNextPhase()
{
        if (currPhaseIndex == 0)
                m_OrbitSystem->CreateSun();
        else if (currPhaseIndex < 4)
        {
                m_OrbitSystem->CreateRing(currPhaseIndex - 1);
        }

        currPhaseIndex += 1;
        if (currPhaseIndex >= E_TUTORIAL_PHASE::COUNT)
        {
                finished = true;
        }

        else
        {
                currPhase = phases[currPhaseIndex];
        }
        m_SpeedBoostSystem->RequestDestroyAllSpeedboosts();

        switch (currPhase)
        {
                        // White Orbs
                case E_TUTORIAL_PHASE::PHASE_1:
                {
                        UpdatePhase1(0.0f);
                }
                break;

                // Red Orbs
                case E_TUTORIAL_PHASE::PHASE_2:
                {
                        UpdatePhase2(0.0f);
                }
                break;

                        // Green Orbs
                case E_TUTORIAL_PHASE::PHASE_3:
                {
                        UpdatePhase3(0.0f);
                }
                break;

                        // Blue Orbs
                case E_TUTORIAL_PHASE::PHASE_4:
                {
                        UpdatePhase4(0.0f);
                }
                break;

                case E_TUTORIAL_PHASE::PHASE_5:
                {
                        UpdatePhase5(0.0f);
                }
                break;
        }
}

void TutorialLevel::UpdatePhase1(float deltaTime)
{
        //UIManager::instance->MainTitleUnpause();
        for (int i = 0; i < 3; ++i)
        {
                m_SpeedBoostSystem->m_ColorsCollected[i] = true;
        }
        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
}

void TutorialLevel::UpdatePhase2(float deltaTime)
{
        UIManager::instance->WhiteOrbCollected();
        // m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->sunHandle);

        m_SpeedBoostSystem->m_ColorsCollected[0] = false;
        for (int i = 1; i < 4; ++i)
        {
                m_SpeedBoostSystem->m_ColorsCollected[i] = true;
        }
}

void TutorialLevel::UpdatePhase3(float deltaTime)
{
        UIManager::instance->RedOrbCollected();

        // m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring1Handle);

        m_SpeedBoostSystem->m_ColorsCollected[0] = true;
        m_SpeedBoostSystem->m_ColorsCollected[1] = false;
        m_SpeedBoostSystem->m_ColorsCollected[2] = true;
        m_SpeedBoostSystem->m_ColorsCollected[3] = true;
}

void TutorialLevel::UpdatePhase4(float deltaTime)
{
        UIManager::instance->GreenOrbCollected();

        // m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring2Handle);

        m_SpeedBoostSystem->m_ColorsCollected[0] = true;
        m_SpeedBoostSystem->m_ColorsCollected[1] = true;
        m_SpeedBoostSystem->m_ColorsCollected[2] = false;
        m_SpeedBoostSystem->m_ColorsCollected[3] = true;
}

void TutorialLevel::UpdatePhase5(float deltaTime)
{
        UIManager::instance->BlueOrbCollected();

        m_SpeedBoostSystem->m_ColorsCollected[0] = false;
        m_SpeedBoostSystem->m_ColorsCollected[1] = false;
        m_SpeedBoostSystem->m_ColorsCollected[2] = false;
        m_SpeedBoostSystem->m_ColorsCollected[3] = true;

        //->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);
        levelRequested += 1;
        GEngine::Get()->GetLevelStateManager()->RequestState(E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_01);
}

TutorialLevel* TutorialLevel::Get()
{
        return Instance;
}
