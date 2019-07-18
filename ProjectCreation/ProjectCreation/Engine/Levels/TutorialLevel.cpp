#include "TutorialLevel.h"

using namespace DirectX;

class SpeedBoostSystem;
class SpeedBoostComponent;

void TutorialLevel::Enter()
{
        // Update(GEngine::Get()->GetDeltaTime());
}

void TutorialLevel::Update(float deltaTime)
{

        if (m_WhiteCollected == false && whiteCount <= 0)
        {
                SpawnFirstWhiteOrb();
        }

        else if (m_RedCollected == false && redCount <= 0 && m_WhiteCollected == true)
        {
                SpawnFirstRedOrb();
        }

        else if (m_BlueCollected == false && blueCount <= 0 && m_RedCollected == true)
        {
                SpawnFirstBlueOrb();
        }

        else if (m_GreenCollected == false && greenCount <= 0 && m_BlueCollected == true)
        {
                SpawnFirstGreenOrb();
        }

        for (auto& speedComp : m_HandleManager->GetActiveComponents<SpeedboostComponent>())
        {
                XMVECTOR center = speedComp.GetParent().GetComponent<TransformComponent>()->transform.translation;

                XMVECTOR dir        = center - m_PlayerTransform->transform.translation;
                float    distanceSq = MathLibrary::VectorDotProduct(dir, dir);

                float checkRadius = speedComp.collisionRadius;

                if (speedComp.lifetime > 0.0f && distanceSq < (checkRadius * checkRadius) && m_WhiteCollected == false)
                {
                        ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                        XMVECTOR pos = m_PlayerTransform->transform.translation + 2.0f * VectorConstants::Forward;

                        int count = 1;
                        if (count >= 1)
                        {
                                m_WhiteCollected = true;
                        }
                        m_SpeedBoostSystem->RequestDestroySpeedboost(&speedComp);
                        break;
                }

                if (speedComp.lifetime > 0.0f && distanceSq < (checkRadius * checkRadius) && m_WhiteCollected == true && m_RedCollected == false)
                {
                        ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                        XMVECTOR pos = m_PlayerTransform->transform.translation + 2.0f * VectorConstants::Forward;

                        int count = 1;
                        if (count >= 1)
                        {
                                m_RedCollected = true;
                        }
                        m_SpeedBoostSystem->RequestDestroySpeedboost(&speedComp);
                        break;
                }

                if (speedComp.lifetime > 0.0f && distanceSq < (checkRadius * checkRadius) && m_RedCollected == true && m_BlueCollected == false)
                {
                        ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                        XMVECTOR pos = m_PlayerTransform->transform.translation + 2.0f * VectorConstants::Forward;

                        int count = 1;
                        if (count >= 1)
                        {
                                m_BlueCollected = true;
                        }
                        m_SpeedBoostSystem->RequestDestroySpeedboost(&speedComp);
                        break;
                }

                if (speedComp.lifetime > 0.0f && distanceSq < (checkRadius * checkRadius) && m_BlueCollected == true && m_GreenCollected == false)
                {
                        ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                        XMVECTOR pos = m_PlayerTransform->transform.translation + 2.0f * VectorConstants::Forward;

                        int count = 1;
                        if (count >= 1)
                        {
                                m_GreenCollected = true;
                        }
                        m_SpeedBoostSystem->RequestDestroySpeedboost(&speedComp);
                        break;
                }
        }

        if ((m_WhiteCollected == true && m_RedCollected == true && m_BlueCollected == true && m_GreenCollected == true) && levelRequested <= 0)
        {
                levelRequested += 1;
                GEngine::Get()->GetLevelStateManager()->RequestNextLevel();
        }
}


void TutorialLevel::Exit()
{}

TutorialLevel::TutorialLevel()
{
        m_LevelType        = E_Level_States::TUTORIAL_LEVEL;
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();
        whiteCount = redCount = blueCount = greenCount = levelRequested = 0;

        m_PlayerEntityHandle = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                   ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                   ->GetControlledEntity();


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

        XMVECTOR pos    = m_PlayerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::WHITE_LIGHTS);

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

        XMVECTOR pos    = m_PlayerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::RED_LIGHTS);

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

        XMVECTOR pos    = m_PlayerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::BLUE_LIGHTS);

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

        XMVECTOR pos    = m_PlayerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::GREEN_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::GREEN_LIGHTS;
}
