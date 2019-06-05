#include "SpeedBoostSystem.h"
#include "../Controller/ControllerManager.h"
#include "../Entities/EntityFactory.h"
#include "../GEngine.h"

#include "..//GenericComponents/TransformComponent.h"
#include "GoalComponent.h"
#include "SpeedboostComponent.h"

#include <cmath>
#include <map>
#include <random>
#include <string>
#include "..//CoreInput/CoreInput.h"
#include "..//MathLibrary/MathLibrary.h"
#include "../Controller/PlayerMovement.h"

using namespace DirectX;

std::random_device                    r;
std::default_random_engine            e1(r());
std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);

void SpeedBoostSystem::RespawnSpeedBoost(TransformComponent*       boostTC,
                                         SpeedboostComponent*      boostSC,
                                         const TransformComponent* playerTC,
                                         const TransformComponent* targetTC)
{
        float x     = 0.25f * m_MaxBoostDistance * (uniform_dist(e1) * 2.0f - 1.0f);
        float alpha = std::max(uniform_dist(e1), 0.1f);

        XMVECTOR dir = XMVector3Normalize(targetTC->transform.translation - playerTC->transform.translation);
        dir          = XMVector3Cross(dir, VectorConstants::Up);

        XMVECTOR target = XMVectorLerp(playerTC->transform.translation, targetTC->transform.translation, alpha) + dir * x;
        XMVECTOR limit  = XMVectorSet(m_MaxBoostDistance, 0.0f, m_MaxBoostDistance, 0.0f);
        target = XMVectorClamp(target, playerTC->transform.translation - limit, playerTC->transform.translation + limit);
        boostTC->transform.translation = target;
        boostSC->m_TargetRadius        = m_BoostRadius;

        boostSC->m_WantsRespawn = false;
        boostSC->m_Lifetime     = m_BoostLifespan;
        boostSC->m_WantsRespawn = false;
}

void SpeedBoostSystem::SpawnSpeedBoost(const TransformComponent* playerTC, const TransformComponent* targetTC)
{
        float distance = MathLibrary::CalulateDistanceSq(playerTC->transform.translation, targetTC->transform.translation);

        if (distance < (m_MaxBoostDistance / 2) * (m_MaxBoostDistance / 2))
                return;

        ComponentHandle transCompHandle;
        auto            eh               = EntityFactory::CreateStaticMeshEntity("Sphere01", "GlowMatRing", &transCompHandle);
        auto            speedboostHandle = m_ComponentManager->AddComponent<SpeedboostComponent>(eh);

        auto                sc = m_ComponentManager->GetComponent<SpeedboostComponent>(speedboostHandle);
        TransformComponent* tc = m_ComponentManager->GetComponent<TransformComponent>(transCompHandle);
        RespawnSpeedBoost(tc, sc, playerTC, targetTC);
        tc->transform.SetScale(0.0f);
        sc->m_CurrentRadius = 0.0f;
}

void SpeedBoostSystem::OnPreUpdate(float deltaTime)
{}

void SpeedBoostSystem::OnUpdate(float deltaTime)
{

        TransformComponent* playerTransform = m_ComponentManager->GetComponent<TransformComponent>(
            ControllerManager::Get()->m_Controllers[ControllerManager::E_CONTROLLERS::PLAYER]->GetControlledEntity());

        GEngine::Get()->m_PlayerRadius = MathLibrary::lerp(GEngine::Get()->m_PlayerRadius, m_PlayerEffectRadius, deltaTime);

        TransformComponent* closestGoalTransform = nullptr;

        if (!m_ComponentManager->ComponentsExist<GoalComponent>())
        {

                auto goalCompItr = m_ComponentManager->GetActiveComponents<GoalComponent>();
                for (auto itr = goalCompItr.begin(); itr != goalCompItr.end(); itr++)
                {
                        TransformComponent* transComp =
                            m_ComponentManager->GetComponent<TransformComponent>(itr.data()->GetOwner());

                        if (closestGoalTransform == nullptr ||
                            MathLibrary::CalulateDistanceSq(transComp->transform.translation,
                                                            playerTransform->transform.translation) <
                                MathLibrary::CalulateDistanceSq(closestGoalTransform->transform.translation,
                                                                playerTransform->transform.translation))
                        {
                                closestGoalTransform = transComp;
                        }
                }
        }

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


                        float distanceSq = MathLibrary::CalulateDistanceSq(playerTransform->transform.translation,
                                                                           transComp->transform.translation);

                        if (speedComp->m_Lifetime < 0.0f || distanceSq > (m_MaxBoostDistance) * (m_MaxBoostDistance))
                        {
                                speedComp->m_WantsRespawn = true;
                                speedComp->m_TargetRadius = 0.0f;
                        }


                        if ((speedComp->m_WantsRespawn == false) && distanceSq < m_BoostRadius * 2.0f * m_BoostRadius)
                        {
                                speedComp->m_WantsRespawn = true;
                                speedComp->m_TargetRadius = 0.0f;
                                m_PlayerEffectRadius += 1.0f;

                                static_cast<PlayerController*>(
                                    ControllerManager::Get()->m_Controllers[ControllerManager::E_CONTROLLERS::PLAYER])
                                    ->SpeedBoost(XMVectorZero());
                        }

                        if (speedComp->m_CurrentRadius != speedComp->m_TargetRadius)
                        {
                                speedComp->m_CurrentRadius = MathLibrary::MoveTowards(
                                    speedComp->m_CurrentRadius, speedComp->m_TargetRadius, m_BoostShrinkSpeed * deltaTime);
                                transComp->transform.SetScale(speedComp->m_CurrentRadius);
                        }

                        speedComp->m_Lifetime -= deltaTime;
                        if (!playerTransform || !closestGoalTransform)
                                continue;

                        if (speedComp->m_WantsRespawn)
                        {
                                if (speedComp->m_CurrentRadius <= 0.0f)
                                {
                                        RespawnSpeedBoost(transComp, speedComp, playerTransform, closestGoalTransform);
                                }
                        }
                }
        }

        if (closestGoalTransform != nullptr && speedboostCount < m_MaxSpeedBoosts)
        {
                if (m_SpawnBoostTimer <= 0)
                {
                        m_SpawnBoostTimer += m_SpawnBoostCD;
                        SpawnSpeedBoost(playerTransform, closestGoalTransform);
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
        m_EntityManager    = GEngine::Get()->GetEntityManager();
        m_ComponentManager = GEngine::Get()->GetComponentManager();
}

void SpeedBoostSystem::OnShutdown()
{}

void SpeedBoostSystem::OnResume()
{}

void SpeedBoostSystem::OnSuspend()
{}
