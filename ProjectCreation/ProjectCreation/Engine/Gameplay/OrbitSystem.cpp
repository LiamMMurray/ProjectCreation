#include "OrbitSystem.h"
#include <limits>
#include "..//Controller/PlayerMovement.h"
#include "..//CoreInput/CoreInput.h"
#include "..//Entities/EntityFactory.h"
#include "..//GEngine.h"
#include "../Controller/ControllerSystem.h"
#include "../GenericComponents/TransformComponent.h"
#include "../ResourceManager/Material.h"
#include "GoalComponent.h"


using namespace DirectX;

void OrbitSystem::CreateGoal(int color, DirectX::XMVECTOR position)
{
        color = std::min(2, color);

        /*** REFACTORING CODE START ***/
        ComponentHandle transHandle, transHandle2;

        auto entityH1   = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[color], &transHandle);
        auto goalHandle = entityH1.AddComponent<GoalComponent>();
        auto goalComp   = goalHandle.Get<GoalComponent>();
        auto transComp  = transHandle.Get<TransformComponent>();

        auto entityH2 = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[color], &transHandle2, nullptr, false);
        auto transComp2           = transHandle2.Get<TransformComponent>();
        goalComp->color           = color;
        goalComp->collisionHandle = transHandle2;
        goalComp->goalTransform.SetScale(50.0f);
        goalComp->initialTransform.SetScale(1.0f);
        goalComp->initialTransform.translation = position;
        goalComp->goalState                    = E_GOAL_STATE::Spawning;
        transComp->transform                   = goalComp->initialTransform;
        transComp->transform.SetScale(0.0f);

        float time = float(GEngine::Get()->GetTotalTime() / (1.0f + color) + color * 3.7792f);
        float x    = sin(time);
        float y    = cos(time);

        XMVECTOR offset1 = XMVectorSet(x, 0, y, 0.0f);

        goalComp->goalTransform.translation = orbitCenter + offset1 * 150.f * (color + 1.0f);
        transComp2->transform               = goalComp->goalTransform;

        /*** REFACTORING CODE END ***/
}

void OrbitSystem::OnPreUpdate(float deltaTime)
{}

void OrbitSystem::OnUpdate(float deltaTime)
{
        PlayerController* playerController = (PlayerController*)SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                 ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER];

        ComponentHandle playerTransformHandle =
            playerController->GetControlledEntity().GetComponentHandle<TransformComponent>();
        TransformComponent* playerTransform = playerTransformHandle.Get<TransformComponent>();

        double totalTime = GEngine::Get()->GetTotalTime();

        m_ClosestGoalTransform = playerTransformHandle;


        TransformComponent* closestGoalTransform = nullptr;

        for (auto& goalComp : m_HandleManager->GetActiveComponents<GoalComponent>())
        {
                EntityHandle        goalParent  = goalComp.GetParent();
                ComponentHandle     transHandle = goalParent.GetComponentHandle<TransformComponent>();
                TransformComponent* transComp   = transHandle.Get<TransformComponent>();

                float distancePrev = closestGoalTransform ?
                                         MathLibrary::CalulateDistanceSq(closestGoalTransform->transform.translation,
                                                                         playerTransform->transform.translation) :
                                         FLT_MAX;

                float distanceNew =
                    MathLibrary::CalulateDistanceSq(transComp->transform.translation, playerTransform->transform.translation);

                if (goalComp.goalState == E_GOAL_STATE::Idle && (closestGoalTransform == nullptr || distanceNew < distancePrev))
                {
                        m_ClosestGoalTransform = transHandle;
                }
        }

        for (auto& goalComp : m_HandleManager->GetActiveComponents<GoalComponent>())
        {
                EntityHandle        goalParent      = goalComp.GetParent();
                ComponentHandle     goalHandle      = goalComp.GetHandle();
                ComponentHandle     transHandle     = goalParent.GetComponentHandle<TransformComponent>();
                TransformComponent* transComp       = transHandle.Get<TransformComponent>();
                TransformComponent* transCompPuzzle = goalComp.collisionHandle.Get<TransformComponent>();

                if (goalComp.goalState == E_GOAL_STATE::Spawning)
                {
                        float scale       = transComp->transform.GetRadius();
                        float targetScale = goalComp.initialTransform.GetRadius();
                        float newScale    = MathLibrary::MoveTowards(scale, targetScale, deltaTime * 0.25f);
                        transComp->transform.SetScale(newScale);

                        if (scale >= targetScale)
                                goalComp.goalState = E_GOAL_STATE::Idle;

                        return;
                }

                float time = float(totalTime / (1.0f + goalComp.color) + goalComp.color * 3.7792f);
                float x    = sin(time);
                float y    = cos(time);

                XMVECTOR offset1 = XMVectorSet(x, 0, y, 0.0f);

                goalComp.goalTransform.translation = orbitCenter + offset1 * 150.f * (goalComp.color + 1.0f);

                transCompPuzzle->transform = goalComp.goalTransform;

                float distanceSq =
                    MathLibrary::CalulateDistanceSq(playerTransform->transform.translation, transComp->transform.translation);

                if (goalComp.goalState == E_GOAL_STATE::Idle && distanceSq < 3.5f)
                {
                        // goalComp.targetAlpha = 1.0f;
                        // playerController->SetGoalComponent(goalComp.GetHandle());
                        goalComp.goalState  = E_GOAL_STATE::InitialTransition;
                        transComp->wrapping = false;
                        playerController->RequestPuzzleMode(goalHandle, orbitCenter, true, 4.0f);
                }

                if (goalComp.goalState == E_GOAL_STATE::Done)
                {
                        goalComp.targetAlpha = 1.0f;

                        float dist  = MathLibrary::CalulateDistance(goalComp.initialTransform.translation,
                                                                   goalComp.goalTransform.translation);
                        float speed = MathLibrary::lerp(
                            goalComp.transitionInitialSpeed, goalComp.transitionFinalSpeed, std::min(1.0f, goalComp.currAlpha));
                        goalComp.currAlpha =
                            MathLibrary::MoveTowards(goalComp.currAlpha, goalComp.targetAlpha, speed * deltaTime * 1.0f / dist);

                        transComp->transform = FTransform::Lerp(
                            goalComp.initialTransform, goalComp.goalTransform, std::min(1.0f, goalComp.currAlpha));
                }
        }

        if (SYSTEM_MANAGER->GetSystem<ControllerSystem>()->GetOrbCount(E_LIGHT_ORBS::RED_LIGHTS) >= 5)
        {
                static bool done = false;


                if (!done)
                {
                        // play sfx when spawned
                        auto redSpawn = AudioManager::Get()->CreateSFX("redPlanetSpawn");
                        redSpawn->SetVolume(0.8f);
                        CreateGoal(0, GoalPositions[0]);
                        redSpawn->Play();
                }

                done = true;
        }

        if (SYSTEM_MANAGER->GetSystem<ControllerSystem>()->GetOrbCount(E_LIGHT_ORBS::BLUE_LIGHTS) >= 5)
        {
                static bool done = false;


                if (!done)
                {
                        CreateGoal(1, GoalPositions[1]);
                        // play sfx when spawned
                        auto blueSpawn = AudioManager::Get()->CreateSFX("bluePlanetSpawn");
                        blueSpawn->SetVolume(0.8f);
                        blueSpawn->Play();
                }


                done = true;
        }

        if (SYSTEM_MANAGER->GetSystem<ControllerSystem>()->GetOrbCount(E_LIGHT_ORBS::GREEN_LIGHTS) >= 5)
        {
                static bool done = false;


                if (!done)
                {
                        // play sfx when spawned
                        auto greenSpawn = AudioManager::Get()->CreateSFX("greenPlanetSpawn");
                        greenSpawn->SetVolume(0.8f);
                        CreateGoal(2, GoalPositions[2]);
                        greenSpawn->Play();
                }
                done = true;
        }

        //<Joseph's Temp Planet Change>

        // for (int i = 0; i < 3; ++i)
        // {
        //         TransformComponent* tc  = m_HandleManager->GetComponentHandle<TransformComponent>(m_PlanetGoals[i]);
        //         TransformComponent* tc2 = m_HandleManager->GetComponentHandle<TransformComponent>(m_PlanetOutlines[i]);
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
        //         TransformComponent* target = m_HandleManager->GetComponentHandle<TransformComponent>(*m_TargetTransforms[i]);
        //
        //         tc->transform = target->transform;
        // }
}

void OrbitSystem::OnPostUpdate(float deltaTime)
{}

void OrbitSystem::OnInitialize()
{
        m_HandleManager = GEngine::Get()->GetHandleManager();

        ComponentHandle sunHandle, ring1Handle, ring2Handle, ring3Handle;
        EntityFactory::CreateStaticMeshEntity("Sphere01", "GlowMatSun", &sunHandle, nullptr, false);
        EntityFactory::CreateStaticMeshEntity("Ring01", "GlowMatRing", &ring1Handle, nullptr, false);
        EntityFactory::CreateStaticMeshEntity("Ring02", "GlowMatRing", &ring2Handle, nullptr, false);
        EntityFactory::CreateStaticMeshEntity("Ring03", "GlowMatRing", &ring3Handle, nullptr, false);

        auto sunTransform   = sunHandle.Get<TransformComponent>();
        auto ring1Transform = ring1Handle.Get<TransformComponent>();
        auto ring2Transform = ring2Handle.Get<TransformComponent>();
        auto ring3Transform = ring3Handle.Get<TransformComponent>();

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

        PlayerController* playerController = (PlayerController*)SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                 ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER];
        m_ClosestGoalTransform = playerController->GetControlledEntity().GetComponentHandle<TransformComponent>();
}

void OrbitSystem::OnShutdown()
{}

void OrbitSystem::OnResume()
{}

void OrbitSystem::OnSuspend()
{}
