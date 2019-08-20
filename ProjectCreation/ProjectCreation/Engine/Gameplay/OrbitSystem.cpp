#include "OrbitSystem.h"
#include <limits>
#include "../../Rendering/Components/StaticMeshComponent.h"
#include "..//..//Rendering/Components/DirectionalLightComponent.h"
#include "..//Controller/PlayerMovement.h"
#include "..//CoreInput/CoreInput.h"
#include "..//Entities/EntityFactory.h"
#include "..//GEngine.h"
#include "../Controller/ControllerSystem.h"
#include "../GenericComponents/TransformComponent.h"
#include "../Particle Systems/EmitterComponent.h"
#include "../ResourceManager/Material.h"
#include "SpeedBoostSystem.h"


using namespace DirectX;

void OrbitSystem::CreateGoal(int color, DirectX::XMVECTOR position)
{
        color = std::min(2, color);

        if (activeGoal.hasActiveGoal)
        {
                activeGoal.activeGoalGround.Free();
                activeGoal.activeGoalOrbit.Free();
        }

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

        activeGoal.hasActiveGoal    = true;
        activeGoal.activeGoalGround = entityH1;
        activeGoal.activeGoalOrbit  = entityH2;
        activeGoal.activeColor      = color;

        // check if player is in range
        // particle fly up
        XMFLOAT4 goalColor;
        XMStoreFloat4(&goalColor, 4.0f * DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::ORB_COLORS[color]));
        goalColor.w                        = 0.4f;
        goalHandle                         = entityH1.AddComponent<EmitterComponent>();
        EmitterComponent* emitterComponent = goalHandle.Get<EmitterComponent>();
        emitterComponent->ParticleFloatUp(
            XMFLOAT3(-0.3f, -0.3f, -0.3f), XMFLOAT3(0.3f, 0.3f, 0.3f), goalColor, goalColor, XMFLOAT4(3.0f, 1.0f, 0.1f, 0.1f));
        emitterComponent->EmitterData.index         = 2;
        emitterComponent->EmitterData.particleScale = XMFLOAT2(0.2f, 0.2f);
        emitterComponent->maxCount                  = 0;
        emitterComponent->spawnRate                 = 0.0f;
        emitterComponent->EmitterData.textureIndex  = 3;
        /*** REFACTORING CODE END ***/
}

void OrbitSystem::CreateTutorialGoal(int color, DirectX::XMVECTOR position)
{
        color = std::min(2, color);

        if (activeGoal.hasActiveGoal)
        {
                activeGoal.activeGoalGround.Free();
                activeGoal.activeGoalOrbit.Free();
        }

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

        activeGoal.hasActiveGoal    = true;
        activeGoal.activeGoalGround = entityH1;
        activeGoal.activeGoalOrbit  = entityH2;
        activeGoal.activeColor      = color;

        // check if player is in range
        // particle fly up
        XMFLOAT4 goalColor;
        XMStoreFloat4(&goalColor, 4.0f * DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::ORB_COLORS[color]));
        goalColor.w                        = 0.4f;
        goalHandle                         = entityH1.AddComponent<EmitterComponent>();
        EmitterComponent* emitterComponent = goalHandle.Get<EmitterComponent>();
        emitterComponent->ParticleFloatUp(
            XMFLOAT3(-0.3f, -0.3f, -0.3f), XMFLOAT3(0.3f, 0.3f, 0.3f), goalColor, goalColor, XMFLOAT4(3.0f, 1.0f, 0.1f, 0.1f));
        emitterComponent->EmitterData.index         = 2;
        emitterComponent->EmitterData.particleScale = XMFLOAT2(0.2f, 0.2f);
        emitterComponent->maxCount                  = 0;
        emitterComponent->spawnRate                 = 0.0f;
        emitterComponent->EmitterData.textureIndex  = 3;
        /*** REFACTORING CODE END ***/}

        void OrbitSystem::UpdateSunAlignedObjects()
        {
                float deltaTime = 0.02f;

                size_t sizeSpawning = sunAlignedTransformsSpawning.size();

                if (sizeSpawning > 0)
                {
                        for (size_t i = sizeSpawning - 1; i >= 0; --i)
                        {
                                TransformComponent* tc = sunAlignedTransformsSpawning[i].Get<TransformComponent>();

                                float currentRadius = tc->transform.GetRadius();
                                if (fabsf(currentRadius - 150.0f) < 0.1f)
                                {
                                        tc->transform.SetScale(150.0f);
                                        sunAlignedTransformsSpawning.erase(sunAlignedTransformsSpawning.begin() + i);
                                        break;
                                }
                                else
                                {
                                        tc->transform.SetScale(
                                            MathLibrary::MoveTowards(currentRadius, 150.0f, deltaTime * 20.0f));
                                        break;
                                }
                        }
                }
                for (auto& h : sunAlignedTransforms)
                {
                        TransformComponent* tc    = h.Get<TransformComponent>();
                        tc->transform.translation = orbitCenter;
                        tc->transform.rotation    = sunRotation;
                }
        }

        void OrbitSystem::OnPreUpdate(float deltaTime)
        {
                PlayerController* playerController = (PlayerController*)SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                         ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER];

                ComponentHandle playerTransformHandle =
                    playerController->GetControlledEntity().GetComponentHandle<TransformComponent>();
                TransformComponent* playerTransform = playerTransformHandle.Get<TransformComponent>();

                sunRotation = GEngine::Get()->m_SunHandle.GetComponent<DirectionalLightComponent>()->m_LightRotation;

                orbitCenter = playerTransform->transform.translation - sunRotation.GetForward() * orbitOffset;

                UpdateSunAlignedObjects();
        }

        void OrbitSystem::OnUpdate(float deltaTime)
        {
                PlayerController* playerController = (PlayerController*)SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                         ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER];

                ComponentHandle playerTransformHandle =
                    playerController->GetControlledEntity().GetComponentHandle<TransformComponent>();
                TransformComponent* playerTransform = playerTransformHandle.Get<TransformComponent>();

                double totalTime = GEngine::Get()->GetTotalTime();

                TransformComponent* closestGoalTransform = nullptr;

                for (auto& goalComp : m_HandleManager->GetActiveComponents<GoalComponent>())
                {
                        EntityHandle        goalParent      = goalComp.GetParent();
                        ComponentHandle     goalHandle      = goalComp.GetHandle();
                        ComponentHandle     transHandle     = goalParent.GetComponentHandle<TransformComponent>();
                        TransformComponent* transComp       = transHandle.Get<TransformComponent>();
                        TransformComponent* transCompPuzzle = goalComp.collisionHandle.Get<TransformComponent>();


                        float time = float(totalTime / (1.0f + goalComp.color) + goalComp.color * 3.7792f);
                        float x    = sin(time);
                        float y    = cos(time);

                        XMVECTOR offset1 = XMVectorSet(x, y, 0, 0.0f);

                        // if (GEngine::Get()->GetLevelStateManager()->GetCurrentLevelState()->GetLevelType() ==
                        // E_Level_States::TUTORIAL_LEVEL)
                        //{
                        //        offset1 = XMVector3Rotate(offset1, sunRotation.data);
                        //
                        //        goalComp.goalTransform.translation = orbitCenter + offset1 * 150.f * (goalComp.color + 1.0f);
                        //        transCompPuzzle->transform         = goalComp.goalTransform;
                        //
                        //        if (goalComp.goalState == E_GOAL_STATE::Spawning)
                        //        {
                        //                float scale       = transComp->transform.GetRadius();
                        //                float targetScale = goalComp.initialTransform.GetRadius();
                        //                float newScale    = MathLibrary::MoveTowards(scale, targetScale, deltaTime * 0.25f);
                        //                transComp->transform.SetScale(newScale);
                        //
                        //                if (scale >= targetScale)
                        //                        goalComp.goalState = E_GOAL_STATE::Idle;
                        //
                        //                return;
                        //        }
                        //
                        //        float distanceSq = MathLibrary::CalulateDistanceSq(playerTransform->transform.translation,
                        //                                                           transComp->transform.translation);
                        //
                        //        auto emitterComponent = goalParent.GetComponent<EmitterComponent>();
                        //        if (goalComp.goalState == E_GOAL_STATE::Idle && distanceSq < 80.0f)
                        //        {
                        //
                        //                emitterComponent->spawnRate = 50.0f;
                        //                emitterComponent->maxCount  = 2048;
                        //        }
                        //        XMVECTOR deltaDistance =
                        //            transCompPuzzle->transform.translation - transComp->transform.translation;
                        //        XMVECTOR offset = 1.0f * XMVector3Normalize(deltaDistance);
                        //        XMStoreFloat3(&emitterComponent->EmitterData.minInitialVelocity, XMVectorZero());
                        //        XMStoreFloat3(&emitterComponent->EmitterData.maxInitialVelocity, +offset);
                        //        XMStoreFloat3(&emitterComponent->EmitterData.acceleration, deltaDistance / 100.0f);
                        //
                        //        if (goalComp.goalState == E_GOAL_STATE::Idle && distanceSq < 3.5f)
                        //        {
                        //                // goalComp.targetAlpha = 1.0f;
                        //                // playerController->SetGoalComponent(goalComp.GetHandle());
                        //                goalComp.goalState  = E_GOAL_STATE::InitialTransition;
                        //                transComp->wrapping = false;
                        //        }
                        //
                        //        if (goalComp.goalState == E_GOAL_STATE::Done)
                        //        {
                        //                goalComp.targetAlpha = 1.0f;
                        //
                        //                float dist  = MathLibrary::CalulateDistance(goalComp.initialTransform.translation,
                        //                                                           goalComp.goalTransform.translation);
                        //
                        //                float speed = MathLibrary::lerp(goalComp.transitionInitialSpeed,
                        //                                                goalComp.transitionFinalSpeed,
                        //                                                std::min(1.0f, goalComp.currAlpha));
                        //                goalComp.currAlpha = MathLibrary::MoveTowards(
                        //                    goalComp.currAlpha, goalComp.targetAlpha, speed * deltaTime * 1.0f / dist);
                        //
                        //                transComp->transform = FTransform::Lerp(goalComp.initialTransform,
                        //                                                        goalComp.goalTransform,
                        //                                                        std::min(1.0f, goalComp.currAlpha));
                        //        }
                        //}

                        offset1 = XMVector3Rotate(offset1, sunRotation.data);

                        goalComp.goalTransform.translation = orbitCenter + offset1 * 150.f * (goalComp.color + 1.0f);
                        transCompPuzzle->transform         = goalComp.goalTransform;

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

                        float distanceSq = MathLibrary::CalulateDistanceSq(playerTransform->transform.translation,
                                                                           transComp->transform.translation);

                        auto emitterComponent = goalParent.GetComponent<EmitterComponent>();
                        if (goalComp.goalState == E_GOAL_STATE::Idle && distanceSq < 80.0f)
                        {

                                emitterComponent->spawnRate = 50.0f;
                                emitterComponent->maxCount  = 2048;
                        }
                        XMVECTOR deltaDistance = transCompPuzzle->transform.translation - transComp->transform.translation;
                        XMVECTOR offset        = 1.0f * XMVector3Normalize(deltaDistance);
                        XMStoreFloat3(&emitterComponent->EmitterData.minInitialVelocity, XMVectorZero());
                        XMStoreFloat3(&emitterComponent->EmitterData.maxInitialVelocity, +offset);
                        XMStoreFloat3(&emitterComponent->EmitterData.acceleration, deltaDistance / 100.0f);

                        if (goalComp.goalState == E_GOAL_STATE::Idle && distanceSq < 3.5f)
                        {
                                // goalComp.targetAlpha = 1.0f;
                                // playerController->SetGoalComponent(goalComp.GetHandle());
                                goalComp.goalState  = E_GOAL_STATE::InitialTransition;
                                transComp->wrapping = false;
                                playerController->RequestPuzzleMode(goalHandle, orbitCenter, true, 4.0f);
                                playerController->SetCollectedPlanetCount(1 + playerController->GetCollectedPlanetCount());
                                SYSTEM_MANAGER->GetSystem<SpeedBoostSystem>()->m_ColorsCollected[goalComp.color] = true;
                        }

                        if (goalComp.goalState == E_GOAL_STATE::Done)
                        {
                                goalComp.targetAlpha = 1.0f;

                                float dist         = MathLibrary::CalulateDistance(goalComp.initialTransform.translation,
                                                                           goalComp.goalTransform.translation);
                                float speed        = MathLibrary::lerp(goalComp.transitionInitialSpeed,
                                                                goalComp.transitionFinalSpeed,
                                                                std::min(1.0f, goalComp.currAlpha));
                                goalComp.currAlpha = MathLibrary::MoveTowards(
                                    goalComp.currAlpha, goalComp.targetAlpha, speed * deltaTime * 1.0f / dist);

                                transComp->transform = FTransform::Lerp(
                                    goalComp.initialTransform, goalComp.goalTransform, std::min(1.0f, goalComp.currAlpha));
                        }
                }

                goalsCollected       = std::min<unsigned int>(goalsCollected, 3);
                XMVECTOR nextGoalPos = playerTransform->transform.translation +
                                       goalDistances[goalsCollected] * playerTransform->transform.rotation.GetForward2D();
                nextGoalPos = XMVectorSetY(nextGoalPos, 0.0f);

                ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
                if (playerController->m_CollectedSplineOrbCount >= (playerController->m_TotalSplineOrbCount - 5) &&
                    (playerController->m_TotalSplineOrbCount > 0))
                {
                        for (int i = 0; i < 3; ++i)
                        {
                                if (controllerSystem->GetCollectOrbEventID(i) != collectEventTimestamps[i])
                                {
                                        collectEventTimestamps[i] = controllerSystem->GetCollectOrbEventID(i);
                                        if (collectedMask[i] == false)
                                        {
                                                if (activeGoal.hasActiveGoal == false || activeGoal.activeColor != i)
                                                { // play sfx when spawned
                                                        CreateGoal(i, nextGoalPos);
                                                }
                                        }
                                }
                        }
                }
        }

        void OrbitSystem::OnPostUpdate(float deltaTime)
        {}

        void OrbitSystem::OnInitialize()
        {
                m_HandleManager   = GEngine::Get()->GetHandleManager();
                m_ResourceManager = GEngine::Get()->GetResourceManager();

                PlayerController* playerController = (PlayerController*)SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                         ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER];

                ComponentHandle playerTransformHandle =
                    playerController->GetControlledEntity().GetComponentHandle<TransformComponent>();
                TransformComponent* playerTransform = playerTransformHandle.Get<TransformComponent>();

                sunRotation = GEngine::Get()->m_SunHandle.GetComponent<DirectionalLightComponent>()->m_LightRotation;

                orbitCenter = playerTransform->transform.translation - sunRotation.GetForward() * orbitOffset;

                ComponentHandle ring1MeshHandle, ring2MeshHandle, ring3MeshHandle;

                std::string ring1MaterialName = "Ring01Mat", ring2MaterialName = "Ring02Mat", ring3MaterialName = "Ring03Mat";


                EntityFactory::CreateStaticMeshEntity("Sphere01", "GlowMatSun", &sunHandle, nullptr, false);
                EntityFactory::CreateStaticMeshEntity("Ring01", "GlowMatRing", &ring1Handle, &ring1MeshHandle, false);
                EntityFactory::CreateStaticMeshEntity("Ring02", "GlowMatRing", &ring2Handle, &ring2MeshHandle, false);
                EntityFactory::CreateStaticMeshEntity("Ring03", "GlowMatRing", &ring3Handle, &ring3MeshHandle, false);

                auto baseRingMaterial = m_ResourceManager->LoadMaterial("GlowMatRing");

                // Red Ring Material
                {
                        auto ring1MaterialHandle = ring1MeshHandle.Get<StaticMeshComponent>()->m_MaterialHandle;

                        ring1MaterialHandle =
                            m_ResourceManager->CopyResource<Material>(baseRingMaterial, ring1MaterialName.c_str());

                        auto ring1Material = m_ResourceManager->GetResource<Material>(
                            ring1MeshHandle.Get<StaticMeshComponent>()->m_MaterialHandle);

                        XMStoreFloat3(&ring1Material->m_SurfaceProperties.emissiveColor,
                                      1.5f * DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::RING_COLORS[0]));
                }

                // Green Ring Material
                {
                        ring2MeshHandle.Get<StaticMeshComponent>()->m_MaterialHandle =
                            m_ResourceManager->CopyResource<Material>(baseRingMaterial, ring2MaterialName.c_str());

                        auto ring2Material = m_ResourceManager->GetResource<Material>(
                            ring2MeshHandle.Get<StaticMeshComponent>()->m_MaterialHandle);

                        XMStoreFloat3(&ring2Material->m_SurfaceProperties.emissiveColor,
                                      1.5f * DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::RING_COLORS[1]));
                }

                // Blue Ring Material
                {
                        ring3MeshHandle.Get<StaticMeshComponent>()->m_MaterialHandle =
                            m_ResourceManager->CopyResource<Material>(baseRingMaterial, ring3MaterialName.c_str());

                        auto ring3Material = m_ResourceManager->GetResource<Material>(
                            ring3MeshHandle.Get<StaticMeshComponent>()->m_MaterialHandle);

                        XMStoreFloat3(&ring3Material->m_SurfaceProperties.emissiveColor,
                                      1.5f * DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::RING_COLORS[2]));
                }

                auto sunTransform   = sunHandle.Get<TransformComponent>();
                auto ring1Transform = ring1Handle.Get<TransformComponent>();
                auto ring2Transform = ring2Handle.Get<TransformComponent>();
                auto ring3Transform = ring3Handle.Get<TransformComponent>();


                sunAlignedTransforms.push_back(sunHandle);
                sunAlignedTransforms.push_back(ring1Handle);
                sunAlignedTransforms.push_back(ring2Handle);
                sunAlignedTransforms.push_back(ring3Handle);

                UpdateSunAlignedObjects();

                sunTransform->transform.SetScale(0.0f);
                ring1Transform->transform.SetScale(0.0f); // radius of 1
                ring2Transform->transform.SetScale(0.0f); // radius of 2
                ring3Transform->transform.SetScale(0.0f); // radius of 3

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
        }

        void OrbitSystem::OnShutdown()
        {}

        void OrbitSystem::OnResume()
        {}

        void OrbitSystem::OnSuspend()
        {}

        void OrbitSystem::DestroyPlanet(GoalComponent* toDestroy)
        {
                toDestroy->collisionHandle.Get<TransformComponent>()->GetParent().Free();
                toDestroy->GetParent().Free();
                activeGoal.hasActiveGoal = false;
        }
