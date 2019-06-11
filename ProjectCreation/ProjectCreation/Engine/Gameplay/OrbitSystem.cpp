#include "OrbitSystem.h"
#include <limits>
#include "..//Controller/PlayerMovement.h"
#include "..//CoreInput/CoreInput.h"
#include "..//Entities/EntityFactory.h"
#include "..//GEngine.h"
#include "../Controller/ControllerManager.h"
#include "../GenericComponents/TransformComponent.h"
#include "../ResourceManager/Material.h"
#include "GoalComponent.h"


using namespace DirectX;

void OrbitSystem::CreateGoal(int n)
{
        static const XMFLOAT4 positions[3] = {
            {0.0f, 0.0f, 20.0f, 1.0f}, {24.0f, 0.0f, 60.0f, 1.0f}, {-16.0f, 0.0f, 60.0f, 1.0f}};

        n = std::min(2, n);

        /*** REFACTORING CODE START ***/
        ComponentHandle transHandle, transHandle2;
        auto            entityH1   = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[n], &transHandle);
        auto            goalHandle = m_ComponentManager->AddComponent<GoalComponent>(entityH1);
        auto            goalComp   = m_ComponentManager->GetComponent<GoalComponent>(goalHandle);
        auto            transComp  = m_ComponentManager->GetComponent<TransformComponent>(transHandle);

        auto entityH2   = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[n], &transHandle2);
        auto transComp2 = m_ComponentManager->GetComponent<TransformComponent>(transHandle2);

        goalComp->collisionHandle = transHandle2;
        goalComp->goalTransform.SetScale(50.0f);
        goalComp->initialTransform.SetScale(1.0f);
        goalComp->initialTransform.translation = XMLoadFloat4(&positions[n]);
        transComp->transform                   = goalComp->initialTransform;
        transComp2->transform                  = goalComp->goalTransform;
        /*** REFACTORING CODE END ***/

        ++n;
}

void OrbitSystem::OnPreUpdate(float deltaTime)
{}

void OrbitSystem::OnUpdate(float deltaTime)
{
        PlayerController* playerController =
            (PlayerController*)ControllerManager::Get()->m_Controllers[ControllerManager::E_CONTROLLERS::PLAYER];

        TransformComponent* playerTransform =
            m_ComponentManager->GetComponent<TransformComponent>(playerController->GetControlledEntity());

        double totalTime = GEngine::Get()->GetTotalTime();

        m_ClosestGoalTransform = playerTransform->GetHandle();

        if (!m_ComponentManager->ComponentsExist<GoalComponent>())
        {
                TransformComponent* closestGoalTransform = nullptr;

                auto goalCompItr = m_ComponentManager->GetActiveComponents<GoalComponent>();
                for (auto itr = goalCompItr.begin(); itr != goalCompItr.end(); itr++)
                {
                        TransformComponent* transComp =
                            m_ComponentManager->GetComponent<TransformComponent>(itr.data()->GetOwner());

                        GoalComponent* goalComp = (GoalComponent*)itr.data();

                        float distancePrev = closestGoalTransform ?
                                                 MathLibrary::CalulateDistanceSq(closestGoalTransform->transform.translation,
                                                                                 playerTransform->transform.translation) :
                                                 FLT_MAX;

                        float distanceNew = MathLibrary::CalulateDistanceSq(transComp->transform.translation,
                                                                            playerTransform->transform.translation);

                        if (goalComp->goalState == E_GOAL_STATE::Idle &&
                            (closestGoalTransform == nullptr || distanceNew < distancePrev))
                        {
                                m_ClosestGoalTransform = transComp->GetHandle();
                        }
                }
        }

        int goalCount = 0;
        if (!m_ComponentManager->ComponentsExist<GoalComponent>())
        {

                auto goalCompItr = m_ComponentManager->GetActiveComponents<GoalComponent>();
                for (auto itr = goalCompItr.begin(); itr != goalCompItr.end(); itr++)
                {
                        TransformComponent* transComp =
                            m_ComponentManager->GetComponent<TransformComponent>(itr.data()->GetOwner());

                        GoalComponent*      goalComp = (GoalComponent*)itr.data();
                        TransformComponent* transCompPuzzle =
                            m_ComponentManager->GetComponent<TransformComponent>(goalComp->collisionHandle);


                        float time = float(totalTime / (1.0f + goalCount) + goalCount * 3.7792f);
                        float x    = sin(time);
                        float y    = cos(time);

                        XMVECTOR offset1 = XMVectorSet(x, 0, y, 0.0f);

                        goalComp->goalTransform.translation =
                            XMVectorSet(0.0f, 1000.0f, 0.0f, 1.0f) + offset1 * 150.f * (goalCount + 1.0f);

                        transCompPuzzle->transform = goalComp->goalTransform;

                        float distanceSq = MathLibrary::CalulateDistanceSq(playerTransform->transform.translation,
                                                                           transComp->transform.translation);

                        if (goalComp->goalState == E_GOAL_STATE::Idle && distanceSq < 3.5f)
                        {
                                // goalComp->targetAlpha = 1.0f;
                                // playerController->SetGoalComponent(goalComp->GetHandle());
                                goalComp->goalState = E_GOAL_STATE::InitialTransition;
                                playerController->RequestPuzzleMode(goalComp->GetHandle(), orbitCenter, true, 4.0f);
                        }

                        if (goalComp->goalState == E_GOAL_STATE::Done)
                        {
                                goalComp->targetAlpha = 1.0f;

                                float dist          = MathLibrary::CalulateDistance(goalComp->initialTransform.translation,
                                                                           goalComp->goalTransform.translation);
                                float speed         = MathLibrary::lerp(goalComp->transitionInitialSpeed,
                                                                goalComp->transitionFinalSpeed,
                                                                std::min(1.0f, goalComp->currAlpha));
                                goalComp->currAlpha = MathLibrary::MoveTowards(
                                    goalComp->currAlpha, goalComp->targetAlpha, speed * deltaTime * 1.0f / dist);

                                transComp->transform = FTransform::Lerp(
                                    goalComp->initialTransform, goalComp->goalTransform, std::min(1.0f, goalComp->currAlpha));
                        }

                        goalCount++;
                }
        }

        if (ControllerManager::Get()->GetOrbCount(E_LIGHT_ORBS::RED_LIGHTS) >= 5)
        {
                static bool done = false;
                if (!done)
                        CreateGoal(0);
                done = true;
        }

        if (ControllerManager::Get()->GetOrbCount(E_LIGHT_ORBS::BLUE_LIGHTS) >= 5)
        {
                static bool done = false;
                if (!done)
                        CreateGoal(1);
                done = true;
        }

        if (ControllerManager::Get()->GetOrbCount(E_LIGHT_ORBS::GREEN_LIGHTS) >= 5)
        {
                static bool done = false;
                if (!done)
                        CreateGoal(2);
                done = true;
        }
        //<Joseph's Temp Planet Change>

        // for (int i = 0; i < 3; ++i)
        // {
        //         TransformComponent* tc  = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetGoals[i]);
        //         TransformComponent* tc2 = m_ComponentManager->GetComponent<TransformComponent>(m_PlanetOutlines[i]);
        //
        //         float distanceSq =
        //             MathLibrary::CalulateDistanceSq(playerTransform->transform.translation, tc->transform.translation);
        //
        //         if (distanceSq < 1.2f)
        //         {
        //                 if (GCoreInput::GetMouseState(MouseCode::LeftClick) == KeyState::DownFirst)
        //                 {
        //                         m_TargetTransforms[i] = &m_PlanetOutlines[i];
        //                 }
        //         }
        //         TransformComponent* target = m_ComponentManager->GetComponent<TransformComponent>(*m_TargetTransforms[i]);
        //
        //         tc->transform = target->transform;
        // }
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
            ring3Transform->transform.translation                                   = orbitCenter;

        sunTransform->transform.SetScale(150.0f);
        ring1Transform->transform.SetScale(150.0f); // radius of 1
        ring2Transform->transform.SetScale(150.0f); // radius of 2
        ring3Transform->transform.SetScale(150.0f); // radius of 3

        //<Joseph's Temp Material Change>

        // Red Light
        auto planetMat01Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowMatPlanet01");
        auto planetMat01       = GEngine::Get()->GetResourceManager()->GetResource<Material>(planetMat01Handle);
        planetMat01->m_SurfaceProperties.diffuseColor  = {0.9f, 0.7f, 0.7f};
        planetMat01->m_SurfaceProperties.emissiveColor = {1.5f, 0.1f, 0.1f};

        // Blue Light
        auto planetMat02Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowMatPlanet02");
        auto planetMat02       = GEngine::Get()->GetResourceManager()->GetResource<Material>(planetMat02Handle);
        planetMat02->m_SurfaceProperties.diffuseColor  = {0.7f, 0.7f, 0.9f};
        planetMat02->m_SurfaceProperties.emissiveColor = {0.01f, 0.1f, 1.5f};

        // Green Light
        auto planetMat03Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowMatPlanet03");
        auto planetMat03       = GEngine::Get()->GetResourceManager()->GetResource<Material>(planetMat03Handle);
        planetMat03->m_SurfaceProperties.diffuseColor  = {0.7f, 0.9f, 0.7f};
        planetMat03->m_SurfaceProperties.emissiveColor = {0.1f, 1.5f, 0.1f};

        PlayerController* playerController =
            (PlayerController*)ControllerManager::Get()->m_Controllers[ControllerManager::E_CONTROLLERS::PLAYER];
        m_ClosestGoalTransform =
            m_ComponentManager->GetComponent<TransformComponent>(playerController->GetControlledEntity())->GetHandle();
}

void OrbitSystem::OnShutdown()
{}

void OrbitSystem::OnResume()
{}

void OrbitSystem::OnSuspend()
{}
