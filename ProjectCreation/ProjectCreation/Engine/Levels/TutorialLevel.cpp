#include "TutorialLevel.h"

using namespace DirectX;

class SpeedBoostSystem;
class SpeedBoostComponent;

void TutorialLevel::Enter()
{
        m_SpeedBoostSystem = GEngine::Get()->GetSystemManager()->GetSystem<SpeedBoostSystem>();
}

void TutorialLevel::Update(float deltaTime)
{
        if (m_WhiteCollected == false)
        {
                SpawnFirstWhiteOrb();

                ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                int count = controllerSystem->GetOrbCount(E_LIGHT_ORBS::WHITE_LIGHTS);
                if (count >= 1)
                {
                        m_WhiteCollected = true;
                }
        }

        else if (m_RedCollected == false)
        {
                SpawnFirstRedOrb();

                ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                int count = controllerSystem->GetOrbCount(E_LIGHT_ORBS::RED_LIGHTS);
                if (count >= 1)
                {
                        m_RedCollected = true;
                }
        }

        else if (m_BlueCollected == false)
        {
                SpawnFirstBlueOrb();

                ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                int count = controllerSystem->GetOrbCount(E_LIGHT_ORBS::BLUE_LIGHTS);
                if (count >= 1)
                {
                        m_BlueCollected = true;
                }
        }

        else if (m_GreenCollected == false)
        {
                SpawnFirstGreenOrb();

                ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                int count = controllerSystem->GetOrbCount(E_LIGHT_ORBS::GREEN_LIGHTS);
                if (count >= 1)
                {
                        m_GreenCollected = true;
                }
        }

        if (m_WhiteCollected == true && m_RedCollected == true && m_BlueCollected == true && m_GreenCollected == true)
        {
                m_SpeedBoostSystem->SetRandomSpawnEnabled(true);
        }
}

void TutorialLevel::Exit()
{}

void TutorialLevel::SpawnFirstWhiteOrb()
{
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* playerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos    = playerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::WHITE_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::WHITE_LIGHTS;
}

void TutorialLevel::SpawnFirstRedOrb()
{
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* playerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos    = playerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::RED_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::RED_LIGHTS;
}

void TutorialLevel::SpawnFirstBlueOrb()
{
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* playerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos    = playerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::BLUE_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::BLUE_LIGHTS;
}

void TutorialLevel::SpawnFirstGreenOrb()
{
        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();


        TransformComponent* playerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos    = playerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = m_SpeedBoostSystem->SpawnLightOrb(pos, E_LIGHT_ORBS::GREEN_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->collisionRadius = 0.1f;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::GREEN_LIGHTS;
}
