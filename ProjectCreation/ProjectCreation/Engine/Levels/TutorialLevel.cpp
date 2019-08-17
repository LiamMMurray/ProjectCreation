#include "TutorialLevel.h"
#include "../../UI/UIManager.h"
#include "../Controller/ControllerSystem.h"

using namespace DirectX;

class SpeedBoostSystem;
class SpeedBoostComponent;


void TutorialLevel::Enter()
{
        // Update(GEngine::Get()->GetDeltaTime());
        GEngine::Get()->SetPlayerRadius(0);
        m_SpeedBoostSystem->SetRandomSpawnEnabled(false);

		m_SpeedBoostSystem->ResetLevel();

        m_PlayerController->SetCollectedPlanetCount(0);

        m_WhiteCollected = m_RedCollected = m_GreenCollected = m_BlueCollected = false;
        whiteCount = redCount = greenCount = blueCount = levelRequested = 0;

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        controllerSys->ResetLightOrbCounters();

        m_LevelType = E_Level_States::TUTORIAL_LEVEL;
}

void TutorialLevel::Update(float deltaTime)
{

        if (m_WhiteCollected == false && whiteCount <= 0)
        {
                SpawnFirstWhiteOrb();
                // UI: Hold Left Mouse Button to Move Forward
        }

        else if (redCount <= 0 && m_WhiteCollected == true)
        {
                UIManager::instance->WhiteOrbCollected();
                m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->sunHandle);
                SpawnFirstRedOrb();
                // UI: Hold A to Collect Red Orbs
        }

        else if (greenCount <= 0 && m_RedCollected == true)
        {
                UIManager::instance->RedOrbCollected();
                m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring1Handle);
                SpawnFirstGreenOrb();
                // UI: Hold D to Collect Green Orbs
        }

        else if (blueCount <= 0 && m_GreenCollected == true)
        {

                UIManager::instance->GreenOrbCollected();
                m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring2Handle);
                SpawnFirstBlueOrb();
                // UI: Hold S to Collect Blue Orbs
        }

        else if (m_GreenCollected == false && greenCount <= 0 && m_BlueCollected == true)
        {
                UIManager::instance->BlueOrbCollected();
                m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);
                // UI: Hold D to Collect Green Orbs
        }

        ControllerSystem* controllerSys = SYSTEM_MANAGER->GetSystem<ControllerSystem>();


        // whiteCount = controllerSys->m_OrbCounts[E_LIGHT_ORBS::WHITE_LIGHTS];
        // redCount   = controllerSys->m_OrbCounts[E_LIGHT_ORBS::RED_LIGHTS];
        // greenCount = controllerSys->m_OrbCounts[E_LIGHT_ORBS::GREEN_LIGHTS];
        // blueCount  = controllerSys->m_OrbCounts[E_LIGHT_ORBS::BLUE_LIGHTS];

        if (controllerSys->m_OrbCounts[E_LIGHT_ORBS::WHITE_LIGHTS] > 0)
        {
                m_WhiteCollected = true;
        }
        if (controllerSys->m_OrbCounts[E_LIGHT_ORBS::RED_LIGHTS] > 0)
        {
                m_RedCollected = true;
        }
        if (controllerSys->m_OrbCounts[E_LIGHT_ORBS::GREEN_LIGHTS] > 0)
        {
                m_GreenCollected = true;
        }
        if (controllerSys->m_OrbCounts[E_LIGHT_ORBS::BLUE_LIGHTS] > 0)
        {
                m_BlueCollected = true;
        }


        if ((m_WhiteCollected == true && m_RedCollected == true && m_BlueCollected == true && m_GreenCollected == true) &&
            levelRequested <= 0)
        {
                UIManager::instance->BlueOrbCollected();
                m_OrbitSystem->sunAlignedTransformsSpawning.push_back(m_OrbitSystem->ring3Handle);
                levelRequested += 1;
                GEngine::Get()->GetLevelStateManager()->RequestState(E_LevelStateEvents::TUTORIAL_LEVEL_TO_LEVEL_01);
        }
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

void TutorialLevel::SpawnFirstWhiteOrb()
{
        whiteCount += 1;
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* m_PlayerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos = m_PlayerTransform->transform.translation + 8.0f * m_PlayerTransform->transform.rotation.GetForward2D();
		auto handle = SpeedBoostSystem::SpawnLightOrb(pos, E_LIGHT_ORBS::WHITE_LIGHTS, .1f);

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
        auto     handle = SpeedBoostSystem::SpawnLightOrb(pos, E_LIGHT_ORBS::RED_LIGHTS, .1f);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::RED_LIGHTS;
}

void TutorialLevel::SpawnFirstBlueOrb()
{
        blueCount += 1;
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* m_PlayerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos = m_PlayerTransform->transform.translation + 12.0f * m_PlayerTransform->transform.rotation.GetForward2D();
        auto     handle = SpeedBoostSystem::SpawnLightOrb(pos, E_LIGHT_ORBS::BLUE_LIGHTS, .1f);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
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
        auto     handle = SpeedBoostSystem::SpawnLightOrb(pos, E_LIGHT_ORBS::GREEN_LIGHTS, .1f);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::GREEN_LIGHTS;
}
