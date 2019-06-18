#include "SpeedBoostSystem.h"
#include <algorithm>
#include "../Controller/ControllerSystem.h"
#include "../Entities/EntityFactory.h"
#include "../GEngine.h"

#include "..//GenericComponents/TransformComponent.h"
#include "GoalComponent.h"
#include "OrbRespawnComponent.h"
#include "OrbitSystem.h"
#include "SpeedboostComponent.h"
#include "SplineElementComponent.h"

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
#include "../MathLibrary/Splines/SplineUtility.h"

#include <limits>

using namespace DirectX;

std::random_device                    r;
std::default_random_engine            e1(r());
std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);


void SpeedBoostSystem::SpawnRandomSpeedBoost()
{
        TransformComponent* playerTransform =
            m_ComponentManager->GetComponent<TransformComponent>(SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                                     ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                                                     ->GetControlledEntity());
        int      color = MathLibrary::GetRandomIntInRange(0, 4);
        XMVECTOR pos =
            MathLibrary::GetRandomPointInRadius2D(playerTransform->transform.translation, 0.5f, m_MaxBoostDistance - 0.5f);
        auto entityH              = SpawnSpeedBoost(pos, color);
        auto spawnCompH           = m_ComponentManager->AddComponent<OrbRespawnComponent>(entityH);
        auto spawnComp            = m_ComponentManager->GetComponent<OrbRespawnComponent>(spawnCompH);
        spawnComp->m_WantsRespawn = false;
        spawnComp->m_Lifetime     = MathLibrary::RandomFloatInRange(m_BoostLifespan - m_BoostLifespanVariance,
                                                                m_BoostLifespan + m_BoostLifespanVariance);

        ++randomOrbCount;
}

void SpeedBoostSystem::SpawnSplineSpeedBoost(const SplineCluster& cluster, unsigned int index, int color, bool tail, bool head)
{
        auto entityH          = SpawnSpeedBoost(cluster.spawnQueue[index], color);
        auto splineH          = m_ComponentManager->AddComponent<SpeedboostSplineComponent>(entityH);
        auto splineComp       = m_ComponentManager->GetComponent<SpeedboostSplineComponent>(splineH);
        splineComp->bHead     = head;
        splineComp->bTail     = tail;
        splineComp->m_CurrPos = cluster.spawnQueue[index];
        splineComp->m_PrevPos = cluster.spawnQueue[index];
        splineComp->m_NextPos = cluster.spawnQueue[index];

        if (!head)
                splineComp->m_PrevPos = cluster.spawnQueue[index - 1];

        if (!tail)
                splineComp->m_NextPos = cluster.spawnQueue[index + 1];
}

EntityHandle SpeedBoostSystem::SpawnSpeedBoost(const DirectX::XMVECTOR& pos, int color)
{
        ComponentHandle boostHandle;
        ComponentHandle transHandle;
        auto            entityHandle = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[color], &boostHandle);
        boostHandle                  = m_ComponentManager->AddComponent<SpeedboostComponent>(entityHandle);

        SpeedboostComponent* gsc = m_ComponentManager->GetComponent<SpeedboostComponent>(boostHandle);
        TransformComponent*  gtc = m_ComponentManager->GetComponent<TransformComponent>(entityHandle);


        gtc->transform.translation = pos;
        gtc->transform.SetScale(0.0f);
        gsc->m_CurrentRadius = 0.0f;
        gsc->m_Color         = color;
        gsc->m_TargetRadius  = m_BoostRadius;

        return entityHandle;
}

void SpeedBoostSystem::UpdateSpeedboostEvents()
{
        ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        PlayerController* pc = (PlayerController*)controllerSystem->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER];

        TransformComponent* playerTransform =
            m_ComponentManager->GetComponent<TransformComponent>(SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                                     ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                                                     ->GetControlledEntity());
        static bool bFirstStage = false;
        {
                if (!bFirstStage)
                {
                        int count = controllerSystem->GetOrbCount(E_LIGHT_ORBS::WHITE_LIGHTS);
                        if (count >= 1)
                        {
                                m_EnableRandomSpawns = true;
                                bFirstStage          = true;
                        }
                }
        }

        auto orbitSystem = SYSTEM_MANAGER->GetSystem<OrbitSystem>();

        static bool goals[4] = {};
        {
                for (int i = 0; i < 3; ++i)
                {
                        if (!goals[i])
                        {
                                int count = controllerSystem->GetOrbCount(i);
                                if (count >= 5)
                                {
                                        XMVECTOR start = playerTransform->transform.translation +
                                                         2.0f * playerTransform->transform.GetForward();

                                        XMVECTOR dir = orbitSystem->GoalPositions[i] - start;

                                        XMVECTOR end = orbitSystem->GoalPositions[i] - 2.0f * XMVector3Normalize(dir);

                                        CreateRandomPath(start, end, i, 4);
                                        goals[i] = true;
                                }
                        }
                }
        }
}

void SpeedBoostSystem::CreateRandomPath(const DirectX::XMVECTOR& start,
                                        const DirectX::XMVECTOR& end,
                                        int                      color,
                                        float                    width,
                                        unsigned int             waveCount,
                                        float                    heightvar)
{
        m_SplineClusterSpawners.emplace_back(SplineCluster());

        FGoodSpline spline(m_Paths[MathLibrary::GetRandomIntInRange(0, PathCount - 1)]);
        float       length   = spline.GetLength() * MathLibrary::CalulateDistance(start, end);
        int         segments = length / m_SplineLengthPerOrb;
        m_SplineClusterSpawners.back().spawnQueue =
            SplineUtility::BakePointsFromSplineAndTransform(spline, start, end, width, segments, waveCount);
        m_SplineClusterSpawners.back().color   = color;
        m_SplineClusterSpawners.back().current = 0;
}

void SpeedBoostSystem::OnPreUpdate(float deltaTime)
{}

void SpeedBoostSystem::OnUpdate(float deltaTime)
{

        TransformComponent* playerTransform =
            m_ComponentManager->GetComponent<TransformComponent>(SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                                     ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                                                     ->GetControlledEntity());
        auto playerController = static_cast<PlayerController*>(
            SYSTEM_MANAGER->GetSystem<ControllerSystem>()->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]);
        GEngine::Get()->m_PlayerRadius = MathLibrary::lerp(GEngine::Get()->m_PlayerRadius, m_PlayerEffectRadius, deltaTime);

        // m_PlayerEffectRadius                       = 25.0f;
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

						// START: Move speed boosts with ai stuff here
						
						// END: Move speed boosts with ai stuff here
                        float distSq = MathLibrary::CalulateDistanceSq(transComp->transform.translation,
                                                                       playerTransform->transform.translation);

                        bool onCD = false;

                        if (speedComp->m_Timer > 0)
                        {
                                onCD = true;
                                speedComp->m_Timer -= deltaTime;
                        }

                        float distanceSq = MathLibrary::CalulateDistanceSq(playerTransform->transform.translation,
                                                                           transComp->transform.translation);

                        float checkRadius = m_BoostRadius * 2.0f;
                        if (!onCD && distanceSq < (checkRadius * checkRadius))
                        {

                                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->IncreaseOrbCount(speedComp->m_Color);

                                // speedComp->m_WantsRespawn = true;
                                // speedComp->m_TargetRadius = 0.0f;
                                // m_PlayerEffectRadius += 1.0f;
                                speedComp->m_Timer = speedComp->m_CD;
                                playerController->SpeedBoost(transComp->transform.translation);
                        }

                        TransformComponent* closestGoalTransform =
                            m_ComponentManager->GetComponent<TransformComponent>(closestGoalTransformHandle);

                        if (speedComp->m_CurrentRadius != speedComp->m_TargetRadius)
                        {
                                speedComp->m_CurrentRadius = MathLibrary::MoveTowards(
                                    speedComp->m_CurrentRadius, speedComp->m_TargetRadius, m_BoostShrinkSpeed * deltaTime);
                                transComp->transform.SetScale(speedComp->m_CurrentRadius);
                        }
                }
        }

        if (!m_ComponentManager->ComponentsExist<OrbRespawnComponent>())
        {

                auto compItr = m_ComponentManager->GetActiveComponents<OrbRespawnComponent>();

                for (auto itr = compItr.begin(); itr != compItr.end(); itr++)
                {
                        auto                 spawnComp = (OrbRespawnComponent*)itr.data();
                        SpeedboostComponent* speedComp =
                            m_ComponentManager->GetComponent<SpeedboostComponent>(itr.data()->GetOwner());
                        TransformComponent* transComp =
                            m_ComponentManager->GetComponent<TransformComponent>(itr.data()->GetOwner());

                        spawnComp->m_Lifetime -= deltaTime;
                        float distanceSq = MathLibrary::CalulateDistanceSqIgnoreY(playerTransform->transform.translation,
                                                                                  transComp->transform.translation);
                        if (spawnComp->m_Lifetime < 0.0f || distanceSq > (m_MaxBoostDistance) * (m_MaxBoostDistance))
                        {
                                spawnComp->m_WantsRespawn = true;
                                speedComp->m_TargetRadius = 0.0f;
                        }


                        if (spawnComp->m_WantsRespawn)
                        {
                                if (speedComp->m_CurrentRadius <= 0.0f)
                                {
                                        XMVECTOR pos =
                                            MathLibrary::GetRandomPointInRadius2D(playerTransform->transform.translation,
                                                                                  m_MaxBoostDistance - 5.2f,
                                                                                  m_MaxBoostDistance - 0.8f);

                                        spawnComp->m_Lifetime =
                                            MathLibrary::RandomFloatInRange(m_BoostLifespan - m_BoostLifespanVariance,
                                                                            m_BoostLifespan + m_BoostLifespanVariance);
                                        transComp->transform.translation = pos;
                                        speedComp->m_TargetRadius        = m_BoostRadius;
                                        spawnComp->m_WantsRespawn        = false;
                                }
                        }
                }
        }

        if (!m_ComponentManager->ComponentsExist<SpeedboostSplineComponent>())
        {
                auto                             compItr = m_ComponentManager->GetActiveComponents<SpeedboostSplineComponent>();
                bool                             canFly  = false;
                static SpeedboostSplineComponent closestComp;
                float                            checkRadius = m_BoostRadius * 4.0f;

                float prevDistance =
                    MathLibrary::CalulateDistanceIgnoreY(closestComp.m_CurrPos, playerTransform->transform.translation);

                for (auto itr = compItr.begin(); itr != compItr.end(); itr++)
                {
                        SpeedboostSplineComponent* splineComp = (SpeedboostSplineComponent*)itr.data();
                        float                      distance =
                            MathLibrary::CalulateDistanceIgnoreY(playerTransform->transform.translation, splineComp->m_CurrPos);

                        if (distance < (checkRadius))
                        {
                                if (playerController->GetUseGravity() == true || prevDistance > distance)
                                        closestComp = *splineComp;

                                playerController->SetUseGravity(false);
                                flyTimer = flyCD;
                                canFly   = true;
                                // break;
                        }
                }
                float closestDistance =
                    MathLibrary::CalulateDistanceIgnoreY(closestComp.m_CurrPos, playerTransform->transform.translation);
                if (!canFly && closestDistance >= checkRadius)
                {
                        flyTimer -= deltaTime;

                        if (flyTimer <= 0.0f)
                                playerController->SetUseGravity(true);
                }

                if (playerController->GetUseGravity() == false)
                {
                        XMVECTOR dirNext = XMVector3Normalize(closestComp.m_NextPos - playerTransform->transform.translation);
                        XMVECTOR dirPrev = XMVector3Normalize(closestComp.m_PrevPos - playerTransform->transform.translation);

                        XMVECTOR dir, next;
                        bool     attached = false;
                        if (MathLibrary::VectorDotProduct(playerTransform->transform.GetForward(), dirNext) > 0.3f)
                        {
                                attached = true;
                                next     = closestComp.m_NextPos;
                        }
                        else if (MathLibrary::VectorDotProduct(playerTransform->transform.GetForward(), dirPrev) > 0.3f)
                        {
                                attached = true;
                                next     = closestComp.m_PrevPos;
                        }

                        if (attached)
                        {
                                XMVECTOR offset = 0.05f * VectorConstants::Up;
                                XMVECTOR closestPointOnLine =
                                    MathLibrary::GetClosestPointFromLine(closestComp.m_PrevPos + offset,
                                                                         closestComp.m_NextPos + offset,
                                                                         playerTransform->transform.translation);
                                float    distanceToLine = MathLibrary::CalulateDistance(closestPointOnLine + offset,
                                                                                     playerTransform->transform.translation);
                                XMVECTOR point = next + offset;
                                dir            = XMVector3Normalize(point - playerTransform->transform.translation);

                                float strength = MathLibrary::clamp(distanceToLine * 3.0f, 0.5f, 8.0f);
                                playerTransform->transform.translation += dir * strength * deltaTime;
                                // playerController->AddCurrentVelocity(dir * 10.0f * deltaTime);
                        }
                }
        }

        for (int i = 0; i < m_SplineClusterSpawners.size(); ++i)
        {
                if (m_SplineClusterSpawners[i].current < m_SplineClusterSpawners[i].spawnQueue.size())
                {
                        bool head = m_SplineClusterSpawners[i].current == 0;
                        bool tail = m_SplineClusterSpawners[i].current == m_SplineClusterSpawners[i].spawnQueue.size() - 1;

                        SpawnSplineSpeedBoost(m_SplineClusterSpawners[i],
                                              m_SplineClusterSpawners[i].current,
                                              m_SplineClusterSpawners[i].color,
                                              tail,
                                              head);

                        m_SplineClusterSpawners[i].current++;
                }
        }

        if (m_EnableRandomSpawns && randomOrbCount < m_MaxSpeedBoosts)
        {
                if (m_SpawnBoostTimer <= 0)
                {
                        m_SpawnBoostTimer = m_SpawnBoostCD;
                        SpawnRandomSpeedBoost();
                }
                else
                {
                        m_SpawnBoostTimer -= deltaTime;
                }
        }

        UpdateSpeedboostEvents();
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

        // Blue Light
        auto speedBoostMat02Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowSpeedboost02");
        auto speedBoostMat02       = GEngine::Get()->GetResourceManager()->GetResource<Material>(speedBoostMat02Handle);

        // Green Light
        auto speedBoostMat03Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowSpeedboost03");
        auto speedBoostMat03       = GEngine::Get()->GetResourceManager()->GetResource<Material>(speedBoostMat03Handle);

        // White Light
        auto speedBoostMat04Handle = GEngine::Get()->GetResourceManager()->LoadMaterial("GlowSpeedboost04");
        auto speedBoostMat04       = GEngine::Get()->GetResourceManager()->GetResource<Material>(speedBoostMat04Handle);

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


        SpawnSpeedBoost(playerTransform->transform.translation + 2.0f * VectorConstants::Forward, E_LIGHT_ORBS::WHITE_LIGHTS);
        /*CreateRandomPath(playerTransform->transform.translation,
                         SYSTEM_MANAGER->GetSystem<OrbitSystem>()->GoalPositions[0],
                         E_LIGHT_ORBS::BLUE_LIGHTS,
                         5.0f,
                         4);*/
}

void SpeedBoostSystem::OnShutdown()
{}

void SpeedBoostSystem::OnResume()
{}

void SpeedBoostSystem::OnSuspend()
{}
