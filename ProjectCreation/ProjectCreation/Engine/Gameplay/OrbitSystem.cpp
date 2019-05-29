#include "OrbitSystem.h"
#include "..//CoreInput/CoreInput.h"
#include "..//Entities/EntityFactory.h"
#include "..//GEngine.h"
#include "../Controller/ControllerManager.h"
#include "../GenericComponents/TransformComponent.h"

using namespace DirectX;

void OrbitSystem::OnPreUpdate(float deltaTime)
{}

void OrbitSystem::OnUpdate(float deltaTime)
{
        TransformComponent* playerTransform = m_ComponentManager->GetComponent<TransformComponent>(
            ControllerManager::Get()->m_Controllers[ControllerManager::E_CONTROLLERS::PLAYER]->GetControlledEntity());

        double totalTime = GEngine::Get()->GetTotalTime();
        for (int i = 0; i < 3; ++i)
        {
                TransformComponent* tc = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetOutlines[i]);

                float time = totalTime / (1.0f + i) + i * 3.7792f;
                float x    = sin(time);
                float y    = cos(time);

                XMVECTOR offset1 = XMVectorSet(x, 0, y, 0.0f);

                tc->transform.translation = XMVectorSet(0.0f, 1000.0f, 0.0f, 1.0f) + offset1 * 150.f * (i + 1.0f);
        }


        for (int i = 0; i < 3; ++i)
        {
                TransformComponent* tc  = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetGoals[i]);
                TransformComponent* tc2 = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetOutlines[i]);

                float distanceSq =
                    MathLibrary::CalulateDistanceSq(playerTransform->transform.translation, tc->transform.translation);

                if (distanceSq < 1.2f)
                {
                        if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::DownFirst)
                        {
                                m_TargetTransforms[i] = &m_PlanetOutlines[i];
                        }
                }
                TransformComponent* target = m_ComponentManager->GetComponent<TransformComponent>(*m_TargetTransforms[i]);

                tc->transform = target->transform;
        }
}

void OrbitSystem::OnPostUpdate(float deltaTime)
{}

void OrbitSystem::OnInitialize()
{
        m_EntityManager    = GEngine::Get()->GetEntityManager();
        m_ComponentManager = GEngine::Get()->GetComponentManager();

        ComponentHandle sunHandle, ring1Handle, ring2Handle, ring3Handle;
        EntityFactory::CreateStaticMeshEntity("Sphere01", "GlowMatSun", &sunHandle);
        EntityFactory::CreateStaticMeshEntity("Ring01", "GlowMatRing", &ring1Handle);
        EntityFactory::CreateStaticMeshEntity("Ring02", "GlowMatRing", &ring2Handle);
        EntityFactory::CreateStaticMeshEntity("Ring03", "GlowMatRing", &ring3Handle);

        auto sunTransform   = m_ComponentManager->GetComponent<TransformComponent>(sunHandle);
        auto ring1Transform = m_ComponentManager->GetComponent<TransformComponent>(ring1Handle);
        auto ring2Transform = m_ComponentManager->GetComponent<TransformComponent>(ring2Handle);
        auto ring3Transform = m_ComponentManager->GetComponent<TransformComponent>(ring3Handle);

        sunTransform->transform.translation = ring1Transform->transform.translation = ring2Transform->transform.translation =
            ring3Transform->transform.translation                                   = XMVectorSet(0.0f, 1000.0f, 0.0f, 1.0f);

        sunTransform->transform.SetScale(150.0f);
        ring1Transform->transform.SetScale(150.0f); // radius of 1
        ring2Transform->transform.SetScale(150.0f); // radius of 2
        ring3Transform->transform.SetScale(150.0f); // radius of 3

        EntityFactory::CreateDummyTransformEntity(&m_PlanetOutlines[0]);
        EntityFactory::CreateDummyTransformEntity(&m_PlanetOutlines[1]);
        EntityFactory::CreateDummyTransformEntity(&m_PlanetOutlines[2]);

        TransformComponent* planet1 = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetOutlines[0]);
        TransformComponent* planet2 = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetOutlines[1]);
        TransformComponent* planet3 = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetOutlines[2]);
        planet1->transform.SetScale(50.0f);
        planet2->transform.SetScale(50.0f);
        planet3->transform.SetScale(50.0f);

        EntityFactory::CreateStaticMeshEntity("Sphere01", "GlowMatSun", &m_PlanetGoals[0]);
        EntityFactory::CreateStaticMeshEntity("Sphere01", "GlowMatSun", &m_PlanetGoals[1]);
        EntityFactory::CreateStaticMeshEntity("Sphere01", "GlowMatSun", &m_PlanetGoals[2]);

        TransformComponent* goal1 = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetGoals[0]);
        TransformComponent* goal2 = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetGoals[1]);
        TransformComponent* goal3 = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetGoals[2]);

        goal1->transform.SetScale(1.0f);
        goal1->transform.translation = XMVectorSet(0.0f, 0.0f, 20.0f, 1.0f);

        goal2->transform.SetScale(1.0f);
        goal2->transform.translation = XMVectorSet(24.0f, 0.0f, 60.0f, 1.0f);

        goal3->transform.SetScale(1.0f);
        goal3->transform.translation = XMVectorSet(-16.0f, 0.0f, 60.0f, 1.0f);

        m_TargetTransforms[0] = &m_PlanetGoals[0];
        m_TargetTransforms[1] = &m_PlanetGoals[1];
        m_TargetTransforms[2] = &m_PlanetGoals[2];
}

void OrbitSystem::OnShutdown()
{}

void OrbitSystem::OnResume()
{}

void OrbitSystem::OnSuspend()
{}
