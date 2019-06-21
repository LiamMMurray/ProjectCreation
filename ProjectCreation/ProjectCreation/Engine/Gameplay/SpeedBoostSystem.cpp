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


EntityHandle SpeedBoostSystem::SpawnSpeedOrb()
{
        EntityHandle controlledEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                            ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                            ->GetControlledEntity();

        TransformComponent* playerTransform  = controlledEntity.GetComponent<TransformComponent>();
        XMFLOAT3            euler            = playerTransform->transform.rotation.ToEulerAngles();
        euler.x                              = 0.0f;
        FQuaternion quat                     = FQuaternion::FromEulerAngles(euler);
        int         color                    = MathLibrary::GetRandomIntInRange(0, 4);
        XMVECTOR    pos                      = MathLibrary::GetRandomPointInArc(playerTransform->transform.translation,
                                                        quat.GetForward(),
                                                        VectorConstants::Up,
                                                        m_SpawnAngle,
                                                        m_MinSpawnDistance,
                                                        m_MaxSpawnDistance);
        pos                                  = XMVectorSetY(pos, 0.0f);
        auto entityH                         = SpawnLightOrb(pos, color);
        auto speedBoostHandle                = entityH.AddComponent<SpeedboostComponent>();
        auto speedboostComponent             = speedBoostHandle.Get<SpeedboostComponent>();
        speedboostComponent->center          = pos;
        speedboostComponent->collisionRadius = m_BoostRadius;
        speedboostComponent->lifetime        = MathLibrary::RandomFloatInRange(m_BoostLifespan - m_BoostLifespanVariance,
                                                                        m_BoostLifespan + m_BoostLifespanVariance);
        speedboostComponent->decay           = 1.0f;
        speedboostComponent->color           = color;

        return entityH;
}

EntityHandle SpeedBoostSystem::SpawnSplineOrb(const SplineCluster& cluster, int clusterID, bool tail, bool head)
{
        auto entityH          = SpawnLightOrb(cluster.spawnQueue[cluster.current], cluster.color);
        auto splineH          = entityH.AddComponent<SpeedboostSplineComponent>();
        auto splineComp       = splineH.Get<SpeedboostSplineComponent>();
        splineComp->bHead     = head;
        splineComp->bTail     = tail;
        splineComp->m_CurrPos = cluster.spawnQueue[cluster.current];
        splineComp->m_PrevPos = cluster.spawnQueue[cluster.current];
        splineComp->m_NextPos = cluster.spawnQueue[cluster.current];
        splineComp->clusterID = clusterID;

        if (!head)
                splineComp->m_PrevPos = cluster.spawnQueue[cluster.current - 1];

        if (!tail)
                splineComp->m_NextPos = cluster.spawnQueue[cluster.current + 1];

        return entityH;
}

EntityHandle SpeedBoostSystem::SpawnLightOrb(const DirectX::XMVECTOR& pos, int color)
{
        ComponentHandle orbHandle;
        ComponentHandle transHandle;
        auto            entityHandle = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[color], &orbHandle);
        orbHandle                    = m_HandleManager->AddComponent<OrbComponent>(entityHandle);

        OrbComponent*       orbComp       = orbHandle.Get<OrbComponent>();
        TransformComponent* transformComp = entityHandle.GetComponent<TransformComponent>();

        transformComp->transform.translation = pos;
        transformComp->transform.SetScale(0.0f);
        orbComp->m_CurrentRadius = 0.0f;
        orbComp->m_Color         = color;
        orbComp->m_TargetRadius  = m_BoostRadius;

        return entityHandle;
}

void SpeedBoostSystem::RequestDestroySpeedboost(SpeedboostComponent* speedComp)
{
        auto orbComp            = speedComp->GetParent().GetComponent<OrbComponent>();
        speedComp->lifetime     = -1.0f;
        orbComp->m_WantsDestroy = true;
        // speedComp->GetHandle().Free();
}


void SpeedBoostSystem::UpdateSpeedboostEvents()
{
        ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();
        PlayerController* pc = (PlayerController*)controllerSystem->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER];
        EntityHandle      controlledEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                            ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                            ->GetControlledEntity();
        TransformComponent* playerTransform = controlledEntity.GetComponent<TransformComponent>();

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
        auto it = m_SplineClusterSpawners.emplace(std::make_pair(m_ClusterCounter, SplineCluster()));

        FGoodSpline spline(m_Paths[MathLibrary::GetRandomIntInRange(0, PathCount - 1)]);
        float       length   = spline.GetLength() * MathLibrary::CalulateDistance(start, end);
        int         segments = length / m_SplineLengthPerOrb;
        it.first->second.spawnQueue =
            SplineUtility::BakePointsFromSplineAndTransform(spline, start, end, width, segments, waveCount);
        it.first->second.color   = color;
        it.first->second.current = 0;
        m_ClusterCounter++;
}

void SpeedBoostSystem::OnPreUpdate(float deltaTime)
{}

void SpeedBoostSystem::OnUpdate(float deltaTime)
{


        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();

        TransformComponent* playerTransform = playerEntity.GetComponent<TransformComponent>();

        auto playerController = static_cast<PlayerController*>(
            SYSTEM_MANAGER->GetSystem<ControllerSystem>()->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]);

        GEngine::Get()->m_PlayerRadius = MathLibrary::lerp(GEngine::Get()->m_PlayerRadius, m_PlayerEffectRadius, deltaTime);

        XMVECTOR flatPlayerForward;
        {
                XMFLOAT3 euler    = playerTransform->transform.rotation.ToEulerAngles();
                euler.x           = 0.0f;
                FQuaternion quat  = FQuaternion::FromEulerAngles(euler);
                flatPlayerForward = quat.GetForward();
        }

        // m_PlayerEffectRadius                       = 25.0f;
        ComponentHandle closestGoalTransformHandle = m_SystemManager->GetSystem<OrbitSystem>()->GetClosestGoalTransform();
        int             speedboostCount            = 0;
        {
                for (auto& speedComp : m_HandleManager->GetActiveComponents<SpeedboostComponent>())
                {
                        speedboostCount++;

                        // START: Move speed boosts with ai stuff here

                        // END: Move speed boosts with ai stuff here
                        XMVECTOR dir        = speedComp.center - playerTransform->transform.translation;
                        float    distanceSq = MathLibrary::VectorDotProduct(dir, dir);

                        float checkRadius = speedComp.collisionRadius;

                        if (speedComp.lifetime > 0.0f && distanceSq < (checkRadius * checkRadius))
                        {
                                if (playerController->SpeedBoost(speedComp.center, speedComp.color))
                                {
                                        RequestDestroySpeedboost(&speedComp);
                                        break;
                                }
                        }

                        if (!m_EnableRandomSpawns)
                                continue;

                        speedComp.lifetime -= deltaTime * speedComp.decay;

                        float angle = MathLibrary::CalculateAngleBetweenVectors(flatPlayerForward, dir);

                        if (speedComp.lifetime <= 0.0f || distanceSq >= (m_DespawnDistance * m_DespawnDistance) ||
                            angle > m_SpawnAngle)
                        {
                                RequestDestroySpeedboost(&speedComp);
                                continue;
                        }
                }
        }

        for (auto& spawnComp : m_HandleManager->GetActiveComponents<OrbComponent>())
        {
                TransformComponent* transComp = spawnComp.GetParent().GetComponent<TransformComponent>();

                if (spawnComp.m_CurrentRadius != spawnComp.m_TargetRadius)
                {
                        spawnComp.m_CurrentRadius = MathLibrary::MoveTowards(
                            spawnComp.m_CurrentRadius, spawnComp.m_TargetRadius, m_BoostShrinkSpeed * deltaTime);
                        transComp->transform.SetScale(spawnComp.m_CurrentRadius);
                }

                if (spawnComp.m_WantsDestroy)
                {
                        spawnComp.m_TargetRadius = 0.0f;
                        if (spawnComp.m_CurrentRadius <= 0.0f)
                        {
                                spawnComp.GetParent().Free();
                        }
                }
        }

        {
                bool                             canFly = false;
                static SpeedboostSplineComponent closestComp;
                float                            checkRadius = m_BoostRadius * 4.0f;

                float prevDistance =
                    MathLibrary::CalulateDistanceIgnoreY(closestComp.m_CurrPos, playerTransform->transform.translation);

                for (auto& splineComp : m_HandleManager->GetActiveComponents<SpeedboostSplineComponent>())
                {
                        float distance =
                            MathLibrary::CalulateDistanceIgnoreY(playerTransform->transform.translation, splineComp.m_CurrPos);

                        if (distance < (checkRadius))
                        {
                                if (playerController->GetUseGravity() == true || prevDistance > distance)
                                        closestComp = splineComp;

                                playerController->SetUseGravity(false);
                                flyTimer = flyCD;
                                canFly   = true;
                                // splineComp.GetParent().Free();
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
                                XMVECTOR offset = 0.02f * VectorConstants::Up;
                                XMVECTOR closestPointOnLine =
                                    MathLibrary::GetClosestPointFromLine(closestComp.m_PrevPos + offset,
                                                                         closestComp.m_NextPos + offset,
                                                                         playerTransform->transform.translation);
                                float    distanceToLine = MathLibrary::CalulateDistance(closestPointOnLine + offset,
                                                                                     playerTransform->transform.translation);
                                XMVECTOR point          = next + offset;
                                dir                     = XMVector3Normalize(point - playerTransform->transform.translation);
                                float targetY           = XMVectorGetY(point);
                                float strength          = MathLibrary::clamp(distanceToLine * 3.0f, 0.5f, 8.0f);
                                playerTransform->transform.translation += dir * strength * deltaTime;
                                // playerController->AddCurrentVelocity(dir * 10.0f * deltaTime);
                        }
                }
        }

        for (auto& it : m_SplineClusterSpawners)
        {
                if (it.second.current < it.second.spawnQueue.size())
                {
                        bool head = it.second.current == 0;
                        bool tail = it.second.current == it.second.spawnQueue.size() - 1;

                        SpawnSplineOrb(it.second, it.first, tail, head);

                        it.second.current++;
                }
        }

        if (m_EnableRandomSpawns && speedboostCount < m_MaxSpeedBoosts)
        {
                if (m_SpawnBoostTimer <= 0)
                {
                        m_SpawnBoostTimer = m_SpawnBoostCD;
                        SpawnSpeedOrb();
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
        m_HandleManager = GEngine::Get()->GetHandleManager();
        m_SystemManager = GEngine::Get()->GetSystemManager();

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

        EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                        ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                        ->GetControlledEntity();

        TransformComponent* playerTransform = playerEntity.GetComponent<TransformComponent>();

        XMVECTOR pos    = playerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        auto     handle = SpawnLightOrb(pos, E_LIGHT_ORBS::WHITE_LIGHTS);

        auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        speedboostComponent->center          = pos;
        speedboostComponent->collisionRadius = m_BoostRadius;
        speedboostComponent->lifetime        = 1.0f;
        speedboostComponent->decay           = 0.0f;
        speedboostComponent->color           = E_LIGHT_ORBS::WHITE_LIGHTS;
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
