#include "SpeedBoostSystem.h"
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
        boostSC->m_Lifetime     = m_BoostLifespan;
        boostSC->m_WantsRespawn = false;
}

void SpeedBoostSystem::SpawnSpeedBoost(const TransformComponent* playerTC,
                                       const TransformComponent* targetTC,
                                       E_LIGHT_ORBS              color)
{
        ComponentHandle boostHandle;
        ComponentHandle transHandle;
        auto            entityHandle = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[color], &boostHandle);
        boostHandle                  = m_HandleManager->AddComponent<SpeedboostComponent>(entityHandle);

        SpeedboostComponent* gsc = boostHandle.Get<SpeedboostComponent>();
        TransformComponent*  gtc = entityHandle.GetComponent<TransformComponent>();
        RespawnSpeedBoost(gtc, gsc, playerTC, targetTC);
        gtc->transform.SetScale(0.0f);
        gsc->m_CurrentRadius = 0.0f;
        gsc->m_Color         = color;
}

void SpeedBoostSystem::OnPreUpdate(float deltaTime)
{}

void SpeedBoostSystem::OnUpdate(float deltaTime)
{

        TransformComponent* playerTransform = (SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                   ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                                   ->GetControlledEntity())
                                                  .GetComponent<TransformComponent>();

        GEngine::Get()->m_PlayerRadius = MathLibrary::lerp(GEngine::Get()->m_PlayerRadius, m_PlayerEffectRadius, deltaTime);

        ComponentHandle closestGoalTransformHandle = m_SystemManager->GetSystem<OrbitSystem>()->GetClosestGoalTransform();


		int speedboostCount = 0;
        for (auto& speedComp : m_HandleManager->GetActiveComponents<SpeedboostComponent>())
        {
                speedboostCount++;

                TransformComponent* transComp = speedComp.GetParent().GetComponent<TransformComponent>();


                float distanceSq =
                    MathLibrary::CalulateDistanceSq(playerTransform->transform.translation, transComp->transform.translation);

                if (speedComp.m_Lifetime < 0.0f || distanceSq > (m_MaxBoostDistance) * (m_MaxBoostDistance))
                {
                        speedComp.m_WantsRespawn = true;
                        speedComp.m_TargetRadius = 0.0f;
                }


                if ((speedComp.m_WantsRespawn == false) && distanceSq < m_BoostRadius * 2.0f * m_BoostRadius)
                {
                        if (speedComp.m_Color == E_LIGHT_ORBS::RED_LIGHTS)
                        {
                                m_CurRedOrbs++;
                                m_MaxRedOrbs--;
                                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->SetOrbCount(E_LIGHT_ORBS::RED_LIGHTS);
                        }

                        if (speedComp.m_Color == E_LIGHT_ORBS::BLUE_LIGHTS)
                        {
                                m_CurBlueOrbs++;
                                m_MaxBlueOrbs--;
                                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->SetOrbCount(E_LIGHT_ORBS::BLUE_LIGHTS);
                        }
                        if (speedComp.m_Color == E_LIGHT_ORBS::GREEN_LIGHTS)
                        {
                                m_CurGreenOrbs++;
                                m_MaxGreenOrbs--;
                                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->SetOrbCount(E_LIGHT_ORBS::GREEN_LIGHTS);
                        }

                        speedComp.m_WantsRespawn = true;
                        speedComp.m_TargetRadius = 0.0f;
                        m_PlayerEffectRadius += 1.0f;

                        static_cast<PlayerController*>(SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                           ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER])
                            ->SpeedBoost(XMVectorZero());
                }

                TransformComponent* closestGoalTransform = closestGoalTransformHandle.Get<TransformComponent>();

                if (speedComp.m_CurrentRadius != speedComp.m_TargetRadius)
                {
                        speedComp.m_CurrentRadius = MathLibrary::MoveTowards(
                            speedComp.m_CurrentRadius, speedComp.m_TargetRadius, m_BoostShrinkSpeed * deltaTime);
                        transComp->transform.SetScale(speedComp.m_CurrentRadius);
                }

                speedComp.m_Lifetime -= deltaTime;
                if (!playerTransform || !closestGoalTransform)
                        continue;

                if (speedComp.m_WantsRespawn)
                {
                        if (speedComp.m_CurrentRadius <= 0.0f)
                        {
                                RespawnSpeedBoost(transComp, &speedComp, playerTransform, closestGoalTransform);
                        }
                }
        }

        TransformComponent* closestGoalTransform = closestGoalTransformHandle.Get<TransformComponent>();

        if (closestGoalTransform != nullptr && speedboostCount < m_MaxSpeedBoosts)
        {
                if (m_SpawnBoostTimer <= 0)
                {
                        m_SpawnBoostTimer += m_SpawnBoostCD;
                        for (uint32_t redOrbs = 0; redOrbs < m_MaxRedOrbs; ++redOrbs)
                        {
                                SpawnSpeedBoost(playerTransform, closestGoalTransform, E_LIGHT_ORBS::RED_LIGHTS);
                        }

                        for (uint32_t blueOrbs = 0; blueOrbs < m_MaxBlueOrbs; ++blueOrbs)
                        {
                                SpawnSpeedBoost(playerTransform, closestGoalTransform, E_LIGHT_ORBS::BLUE_LIGHTS);
                        }

                        for (uint32_t greenOrbs = 0; greenOrbs < m_MaxGreenOrbs; ++greenOrbs)
                        {
                                SpawnSpeedBoost(playerTransform, closestGoalTransform, E_LIGHT_ORBS::GREEN_LIGHTS);
                        }
                }
                else
                {
                        m_SpawnBoostTimer -= deltaTime;
                }
        }
}

void SpeedBoostSystem::OnPostUpdate(float deltaTime)
{}

void SpeedBoostSystem::OnInitialize()
{
        m_HandleManager = GEngine::Get()->GetHandleManager();
        m_SystemManager = GEngine::Get()->GetSystemManager();

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
}

void SpeedBoostSystem::OnShutdown()
{}

void SpeedBoostSystem::OnResume()
{}

void SpeedBoostSystem::OnSuspend()
{}
