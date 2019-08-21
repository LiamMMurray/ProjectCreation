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
        // Update(GEngine::Get()->GetDeltaTime());
        Instance       = new TutorialLevel();
        currPhaseIndex = 0;
        currPhase      = phases[currPhaseIndex];
        GEngine::Get()->SetPlayerRadius(0);

        m_SpeedBoostSystem->splineWidth  = 1.0f;
        m_SpeedBoostSystem->splineHeight = 0.25f;

        m_SpeedBoostSystem->ResetLevel();

        m_SpeedBoostSystem->inTutorial = true;

        m_PlayerController->SetCollectedPlanetCount(0);

        m_WhiteCollected = m_RedCollected = m_GreenCollected = m_BlueCollected = false;
        whiteCount = redCount = greenCount = blueCount = levelRequested = 0;

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        controllerSys->ResetLightOrbCounters();

        m_LevelType = E_Level_States::TUTORIAL_LEVEL;
}

void TutorialLevel::Update(float deltaTime)
{
        switch (currPhase)
        {
                        // White Orbs
                case E_TUTORIAL_PHASE::PHASE_1:
                {
                        UpdatePhase1(deltaTime);
                }
                break;

                // Red Orbs
                case E_TUTORIAL_PHASE::PHASE_2:
                {
                        UpdatePhase2(deltaTime);
                }
                break;

                        // Green Orbs
                case E_TUTORIAL_PHASE::PHASE_3:
                {
                        UpdatePhase3(deltaTime);
                }
                break;

                        // Blue Orbs
                case E_TUTORIAL_PHASE::PHASE_4:
                {
                        UpdatePhase4(deltaTime);
                }
                break;

                case E_TUTORIAL_PHASE::PHASE_5:
                {
                        UpdatePhase5(deltaTime);
                }
                break;
        }

        // if (m_WhiteCollected == false && whiteCount <= 0)
        //{
        //        SpawnFirstWhiteOrb();
        //        // UI: Hold Left Mouse Button to Move Forward
        //}
        //
        // else if (redCount <= 0 && m_WhiteCollected == true)
        //{
        //        UIManager::instance->WhiteOrbCollected();
        //        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->sunHandle);
        //        SpawnFirstRedOrb();
        //        // UI: Hold A to Collect Red Orbs
        //}
        //
        // else if (greenCount <= 0 && m_RedCollected == true)
        //{
        //        UIManager::instance->RedOrbCollected();
        //        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring1Handle);
        //        SpawnFirstGreenOrb();
        //        // UI: Hold D to Collect Green Orbs
        //}
        //
        // else if (blueCount <= 0 && m_GreenCollected == true)
        //{
        //
        //        UIManager::instance->GreenOrbCollected();
        //        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring2Handle);
        //        SpawnFirstBlueOrb();
        //        // UI: Hold S to Collect Blue Orbs
        //}
        //
        // else if (m_GreenCollected == false && greenCount <= 0 && m_BlueCollected == true)
        //{
        //        UIManager::instance->BlueOrbCollected();
        //        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);
        //        // UI: Hold D to Collect Green Orbs
        //}
        //
        // ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        //
        //
        //// whiteCount = controllerSys->m_OrbCounts[E_LIGHT_ORBS::WHITE_LIGHTS];
        //// redCount   = controllerSys->m_OrbCounts[E_LIGHT_ORBS::RED_LIGHTS];
        //// greenCount = controllerSys->m_OrbCounts[E_LIGHT_ORBS::GREEN_LIGHTS];
        //// blueCount  = controllerSys->m_OrbCounts[E_LIGHT_ORBS::BLUE_LIGHTS];
        //
        // if (controllerSys->m_OrbCounts[E_LIGHT_ORBS::WHITE_LIGHTS] > 0)
        //{
        //        m_WhiteCollected = true;
        //}
        // if (controllerSys->m_OrbCounts[E_LIGHT_ORBS::RED_LIGHTS] > 0)
        //{
        //        m_RedCollected = true;
        //}
        // if (controllerSys->m_OrbCounts[E_LIGHT_ORBS::GREEN_LIGHTS] > 0)
        //{
        //        m_GreenCollected = true;
        //}
        // if (controllerSys->m_OrbCounts[E_LIGHT_ORBS::BLUE_LIGHTS] > 0)
        //{
        //        m_BlueCollected = true;
        //}
        //
        //
        // if ((m_WhiteCollected == true && m_RedCollected == true && m_BlueCollected == true && m_GreenCollected == true) &&
        //    levelRequested <= 0)
        //{
        //        UIManager::instance->BlueOrbCollected();
        //        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);
        //        levelRequested += 1;
        //        GEngine::Get()->GetLevelStateManager()->RequestState(E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_01);
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
}

void TutorialLevel::RequestNextPhase()
{
        currPhaseIndex += 1;
        if (currPhaseIndex >= E_TUTORIAL_PHASE::COUNT)
        {
                finished = true;
        }

        else
        {
                currPhase = phases[currPhaseIndex];
        }
}

void TutorialLevel::UpdatePhase1(float deltaTime)
{
        for (int i = 0; i < 3; ++i)
        {
                m_SpeedBoostSystem->m_ColorsCollected[i] = true;
        }
        m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
}

void TutorialLevel::UpdatePhase2(float deltaTime)
{
        UIManager::instance->WhiteOrbCollected();
        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->sunHandle);

        m_SpeedBoostSystem->m_ColorsCollected[0] = false;
        for (int i = 1; i < 4; ++i)
        {
                m_SpeedBoostSystem->m_ColorsCollected[i] = true;
        }
}

void TutorialLevel::UpdatePhase3(float deltaTime)
{
        UIManager::instance->RedOrbCollected();

        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring1Handle);

        m_SpeedBoostSystem->m_ColorsCollected[0] = true;
        m_SpeedBoostSystem->m_ColorsCollected[1] = false;
        m_SpeedBoostSystem->m_ColorsCollected[2] = true;
        m_SpeedBoostSystem->m_ColorsCollected[3] = true;
}

void TutorialLevel::UpdatePhase4(float deltaTime)
{
        UIManager::instance->GreenOrbCollected();

        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring2Handle);

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

        m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);
        levelRequested += 1;
        GEngine::Get()->GetLevelStateManager()->RequestState(E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_01);
}

void TutorialLevel::SpawnFirstWhiteOrb()
{
        whiteCount += 1;
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* m_PlayerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos = m_PlayerTransform->transform.translation + 8.0f * m_PlayerTransform->transform.rotation.GetForward2D();

        auto handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::WHITE_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::WHITE_LIGHTS;
}

void TutorialLevel::SpawnFirstRedOrb()
{
        redCount += 1;
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* m_PlayerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos = m_PlayerTransform->transform.translation + 12.0f * m_PlayerTransform->transform.rotation.GetForward2D();
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::RED_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::RED_LIGHTS;
        speedboostComponent->hasParticle     = false;
}

void TutorialLevel::SpawnFirstBlueOrb()
{
        blueCount += 1;
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* m_PlayerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos = m_PlayerTransform->transform.translation + 12.0f * m_PlayerTransform->transform.rotation.GetForward2D();
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::BLUE_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->hasParticle     = false;
        speedboostComponent->color           = E_LIGHT_ORBS::BLUE_LIGHTS;
}

void TutorialLevel::SpawnFirstGreenOrb()
{
        greenCount += 1;
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* m_PlayerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos = m_PlayerTransform->transform.translation + 12.0f * m_PlayerTransform->transform.rotation.GetForward2D();
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::GREEN_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::GREEN_LIGHTS;
        speedboostComponent->hasParticle     = false;
}

TutorialLevel* TutorialLevel::Get()
{
        return Instance;
}
