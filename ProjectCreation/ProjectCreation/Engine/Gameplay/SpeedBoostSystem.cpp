#include "SpeedBoostSystem.h"
#include <algorithm>
#include "../Controller/ControllerSystem.h"
#include "../Entities/EntityFactory.h"
#include "../GEngine.h"

#include "..//GenericComponents/TransformComponent.h"
#include "GoalComponent.h"
#include "OrbitSystem.h"
#include "SpeedboostComponent.h"

#include <cmath>
#include <map>
#include <random>
#include <string>
#include "..//CoreInput/CoreInput.h"
#include "..//MathLibrary/MathLibrary.h"
#include "../Controller/PlayerMovement.h"

#include "../ResourceManager/Material.h"

#include "../MathLibrary/Maze/MazeGenerator.h"
#include "../MathLibrary/Splines/FGoodSpline.h"

using namespace DirectX;

std::random_device                    r;
std::default_random_engine            e1(r());
std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);

void SpeedBoostSystem::RespawnSpeedBoost(TransformComponent*       boostTC,
                                         SpeedboostComponent*      boostSC,
                                         const TransformComponent* playerTC,
                                         const TransformComponent* targetTC)
{
        float x     = m_MaxBoostDistance * (uniform_dist(e1) * 2.0f - 1.0f);
        float z     = m_MaxBoostDistance * (uniform_dist(e1) * 2.0f - 1.0f);
        float alpha = uniform_dist(e1);

        XMVECTOR target = XMVectorLerp(playerTC->transform.translation, targetTC->transform.translation, alpha) +
                          XMVectorSet(x, 0.0f, z, 0.0f);
        target                         = XMVectorSetY(target, 0.0f);
        XMVECTOR limit                 = XMVectorSet(m_MaxBoostDistance, 0.0f, m_MaxBoostDistance, 0.0f);
        boostTC->transform.translation = target;
        boostSC->m_TargetRadius        = m_BoostRadius;

        boostSC->m_WantsRespawn = false;
        boostSC->m_Lifetime     = MathLibrary::RandomFloatInRange(m_BoostLifespan - m_BoostLifespanVariance,
                                                              m_BoostLifespan + m_BoostLifespanVariance);
}

void SpeedBoostSystem::SpawnSpeedBoost(DirectX::XMVECTOR position, int color)
{
        ComponentHandle boostHandle;
        ComponentHandle transHandle;
        auto            entityHandle = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[color], &boostHandle);
        boostHandle                  = m_ComponentManager->AddComponent<SpeedboostComponent>(entityHandle);

        SpeedboostComponent* gsc = m_ComponentManager->GetComponent<SpeedboostComponent>(boostHandle);
        TransformComponent*  gtc = m_ComponentManager->GetComponent<TransformComponent>(entityHandle);

		gtc->transform.translation = position;
        gtc->transform.SetScale(0.0f);
        gsc->m_CurrentRadius = 0.0f;
        gsc->m_Color         = color;
        gsc->m_TargetRadius  = m_BoostRadius;
        gsc->m_WantsRespawn  = false;
}

void SpeedBoostSystem::OnPreUpdate(float deltaTime)
{}

void SpeedBoostSystem::OnUpdate(float deltaTime)
{

        TransformComponent* playerTransform =
            m_ComponentManager->GetComponent<TransformComponent>(SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                                     ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                                                     ->GetControlledEntity());

        GEngine::Get()->m_PlayerRadius = MathLibrary::lerp(GEngine::Get()->m_PlayerRadius, m_PlayerEffectRadius, deltaTime);

        ComponentHandle closestGoalTransformHandle = m_SystemManager->GetSystem<OrbitSystem>()->GetClosestGoalTransform();


        int speedboostCount = 0;
        if (!m_ComponentManager->ComponentsExist<SpeedboostComponent>())
        {

                auto speedboostCompItr = m_ComponentManager->GetActiveComponents<SpeedboostComponent>();

                for (auto itr = speedboostCompItr.begin(); itr != speedboostCompItr.end(); itr++)
                {
                        speedboostCount++;

                        SpeedboostComponent* speedComp = static_cast<SpeedboostComponent*>(itr.data());
                        TransformComponent*  transComp =
                            m_ComponentManager->GetComponent<TransformComponent>(itr.data()->GetOwner());


                        float distanceSq = MathLibrary::CalulateDistanceSqIgnoreY(playerTransform->transform.translation,
                                                                           transComp->transform.translation);

                        /*if (speedComp->m_Lifetime < 0.0f || distanceSq > (m_MaxBoostDistance) * (m_MaxBoostDistance))
                        {
                                speedComp->m_WantsRespawn = true;
                                speedComp->m_


                                = 0.0f;
                        }*/


                        if ((speedComp->m_WantsRespawn == false) && distanceSq < m_BoostRadius * 4.0f * m_BoostRadius)
                        {

                                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->IncreaseOrbCount(speedComp->m_Color);

                                speedComp->m_WantsRespawn = true;
                                speedComp->m_TargetRadius = 0.0f;
                                //m_PlayerEffectRadius += 1.0f;

                                static_cast<PlayerController*>(SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                                   ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER])
                                    ->SpeedBoost(XMVectorZero());
                        }

                        TransformComponent* closestGoalTransform =
                            m_ComponentManager->GetComponent<TransformComponent>(closestGoalTransformHandle);

                        if (speedComp->m_CurrentRadius != speedComp->m_TargetRadius)
                        {
                                speedComp->m_CurrentRadius = MathLibrary::MoveTowards(
                                    speedComp->m_CurrentRadius, speedComp->m_TargetRadius, m_BoostShrinkSpeed * deltaTime);
                                transComp->transform.SetScale(speedComp->m_CurrentRadius);
                        }

                        /*speedComp->m_Lifetime -= deltaTime;
                        if (!playerTransform || !closestGoalTransform)
                                continue;

                        if (speedComp->m_WantsRespawn)
                        {
                                if (speedComp->m_CurrentRadius <= 0.0f)
                                {
                                        RespawnSpeedBoost(transComp, speedComp, playerTransform, closestGoalTransform);
                                }
                        }*/
                }
        }

        TransformComponent* closestGoalTransform =
            m_ComponentManager->GetComponent<TransformComponent>(closestGoalTransformHandle);

        /*if (closestGoalTransform != nullptr)
        {
                if (m_SpawnBoostTimer <= 0)
                {
                        m_SpawnBoostTimer += m_SpawnBoostCD;

                        for (int color = 0; color < (int)E_LIGHT_ORBS::COUNT; ++color)
                        {
                                if (currentOrbCounter[color] < m_MaxSpeedBoosts)
                                        SpawnSpeedBoost(playerTransform, closestGoalTransform, color);
                        }
                }
                else
                {
                        m_SpawnBoostTimer -= deltaTime;
                }
        }*/
}

void SpeedBoostSystem::OnPostUpdate(float deltaTime)
{}

void SpeedBoostSystem::OnInitialize()
{
        m_EntityManager    = GEngine::Get()->GetEntityManager();
        m_ComponentManager = GEngine::Get()->GetComponentManager();
        m_SystemManager    = GEngine::Get()->GetSystemManager();

        // Red Light
        auto speedBoostMat01Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowSpeedboost01");
        auto speedBoostMat01       = GEngine::Get()->GetResourceManager()->GetResource<Material>(speedBoostMat01Handle);
        speedBoostMat01->m_SurfaceProperties.diffuseColor  = {0.9f, 0.1f, 0.1f};
        speedBoostMat01->m_SurfaceProperties.emissiveColor = {1.05f, 0.05f, 0.05f};

        // Blue Light
        auto speedBoostMat02Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowSpeedboost02");
        auto speedBoostMat02       = GEngine::Get()->GetResourceManager()->GetResource<Material>(speedBoostMat02Handle);
        speedBoostMat02->m_SurfaceProperties.diffuseColor  = {0.7f, 0.7f, 0.9f};
        speedBoostMat02->m_SurfaceProperties.emissiveColor = {0.01f, 0.1f, 1.125f};

        // Green Light
        auto speedBoostMat03Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowSpeedboost03");
        auto speedBoostMat03       = GEngine::Get()->GetResourceManager()->GetResource<Material>(speedBoostMat03Handle);
        speedBoostMat03->m_SurfaceProperties.diffuseColor  = {0.1f, 0.9f, 0.1f};
        speedBoostMat03->m_SurfaceProperties.emissiveColor = {0.05f, 1.05f, 0.05f};

        // White Light
        auto speedBoostMat04Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowSpeedboost04");
        auto speedBoostMat04       = GEngine::Get()->GetResourceManager()->GetResource<Material>(speedBoostMat04Handle);
        speedBoostMat04->m_SurfaceProperties.diffuseColor  = {0.5f, 0.5f, 0.5f};
        speedBoostMat04->m_SurfaceProperties.emissiveColor = {1.05f, 1.05f, 1.05f};

        MazeGenerator mazeGenerator;
        m_Paths.resize(PathCount);
        for (int i = 0; i < PathCount; ++i)
        {
                m_Paths[i] = mazeGenerator.GeneratePath(8, 5, 2, 0, 2, 7);
        }

        std::random_shuffle(m_Paths.begin(), m_Paths.end());

        TransformComponent* playerTransform =
            m_ComponentManager->GetComponent<TransformComponent>(SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                                     ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                                                     ->GetControlledEntity());

        XMVECTOR   goalPos  = m_SystemManager->GetSystem<OrbitSystem>()->GoalPositions[0];
        XMVECTOR   dir      = goalPos - playerTransform->transform.translation;
        float      distance = MathLibrary::CalulateVectorLength(goalPos);
        XMVECTOR   scale    = XMVectorSet(distance/10.0f, 1.0f, distance, 1.0f);
        FTransform transform;
        transform.translation = playerTransform->transform.translation;
        transform.rotation    = FQuaternion::LookAt(XMVector3Normalize(dir));
        transform.scale       = scale;
        XMMATRIX    mtx       = transform.CreateMatrix();
        FGoodSpline testSpline(m_Paths[0]);
        float       baseLength = testSpline.GetLength();
        for (int i = 0; i < 50; ++i)
        {
                testSpline.AdvanceDistance(baseLength / 50.0f);
                XMVECTOR point = testSpline.GetCurrentPoint();
                point          = XMVector3Transform(point, mtx);
                point          = XMVectorSetY(point, sinf(3.0f * (float)i / 50.0f));
                SpawnSpeedBoost(point, E_LIGHT_ORBS::WHITE_LIGHTS);
        }
}

void SpeedBoostSystem::OnShutdown()
{}

void SpeedBoostSystem::OnResume()
{}

void SpeedBoostSystem::OnSuspend()
{}
