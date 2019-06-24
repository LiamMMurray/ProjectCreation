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

#include "..//CollisionLibary/CollisionLibary.h"

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

EntityHandle SpeedBoostSystem::SpawnSplineOrb(SplineCluster& cluster, int clusterID, bool tail, bool head)
{
        XMVECTOR prev, curr, next;
        cluster.BakeNextPointOnSpline(prev, curr, next);

        auto entityH      = SpawnLightOrb(curr, cluster.color);
        auto splineH      = entityH.AddComponent<SpeedboostSplineComponent>();
        auto splineComp   = splineH.Get<SpeedboostSplineComponent>();
        splineComp->bHead = head;
        splineComp->bTail = tail;

        splineComp->m_PrevPos = prev;
        splineComp->m_CurrPos = curr;
        splineComp->m_NextPos = next;
        splineComp->clusterID = clusterID;

        if (head)
        {
                XMVECTOR dir  = splineComp->m_CurrPos - splineComp->m_NextPos;
                float    dist = MathLibrary::CalulateDistance(splineComp->m_NextPos, splineComp->m_CurrPos);

                splineComp->m_PrevPos = splineComp->m_CurrPos + dir * dist;
        }

        if (tail)
        {
                XMVECTOR dir  = splineComp->m_CurrPos - splineComp->m_PrevPos;
                float    dist = MathLibrary::CalulateDistance(splineComp->m_PrevPos, splineComp->m_CurrPos);

                splineComp->m_NextPos = splineComp->m_CurrPos + dir * dist;
        }

        return entityH;
}

EntityHandle SpeedBoostSystem::SpawnLightOrb(const DirectX::XMVECTOR& pos, int color)
{
        ComponentHandle orbHandle;
        ComponentHandle transHandle;
        auto            entityHandle =
            EntityFactory::CreateStaticMeshEntity("Sphere01", speedboostMaterialNames[color].c_str(), &orbHandle);
        orbHandle = m_HandleManager->AddComponent<OrbComponent>(entityHandle);

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

                                        CreateRandomPath(start, end, i);
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
        FGoodSpline spline(m_Paths[MathLibrary::GetRandomIntInRange(0, PathCount - 1)]);
        float       length   = spline.GetLength() * MathLibrary::CalulateDistance(start, end);
        int         segments = length / m_SplineLengthPerOrb;
        auto        it       = m_SplineClusterSpawners.emplace(std::make_pair(m_ClusterCounter, SplineCluster(spline)));


        XMVECTOR   dir      = end - start;
        float      distance = MathLibrary::CalulateVectorLength(dir);
        XMVECTOR   scale    = XMVectorSet(width, 1.0f, distance, 1.0f);
        FTransform transform;
        transform.translation        = start;
        transform.rotation           = FQuaternion::LookAt(XMVector3Normalize(dir));
        transform.scale              = scale;
        it.first->second.transform   = transform.CreateMatrix();
        it.first->second.segments    = length / m_SplineLengthPerOrb;
        it.first->second.spiralCount = waveCount;
        it.first->second.maxHeight   = heightvar;
        it.first->second.color       = color;
        it.first->second.current     = 0;
        it.first->second.spawnTimer = it.first->second.spawnCD = m_SpawnBoostCD;
        XMVECTOR dummy;
        it.first->second.BakeNextPointOnSpline(dummy, dummy, dummy);
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

                        float angle         = MathLibrary::CalculateAngleBetweenVectors(flatPlayerForward, dir);
                        float checkDistance = m_DespawnDistanceOffset + m_MaxSpawnDistance;
                        if (speedComp.lifetime <= 0.0f || distanceSq >= (checkDistance * checkDistance) || angle > m_SpawnAngle)
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
                const XMVECTOR offset = m_SplineHeightOffset * VectorConstants::Up;

                static SpeedboostSplineComponent  closestComp;
                static SpeedboostSplineComponent* closestCompPtr = nullptr;
                static SpeedboostSplineComponent  closestFriendComp;
                float                             checkRadius = m_SplineLatchRadius;
                bool                              foundFriend = false;
                float                             prevDistance =
                    MathLibrary::CalulateDistance(closestComp.m_CurrPos, playerTransform->transform.translation);
                for (auto& splineComp : m_HandleManager->GetActiveComponents<SpeedboostSplineComponent>())
                {
                        float distance =
                            MathLibrary::CalulateDistanceIgnoreY(playerTransform->transform.translation, splineComp.m_CurrPos);

                        if (distance < (checkRadius))
                        {
                                if (playerController->GetUseGravity() == true || prevDistance > distance)
                                {
                                        closestComp    = splineComp;
                                        closestCompPtr = &closestComp;
                                        //break;
                                        if (playerController->GetUseGravity() == true ||
                                            closestComp.clusterID == splineComp.clusterID)
                                        {
                                                foundFriend       = true;
                                                closestFriendComp = splineComp;
                                        }
                                }
                        }
                }

                if (foundFriend)
                        closestComp = closestFriendComp;

                bool onGravityEnable = playerController->GetUseGravity();
                if (closestCompPtr)
                {
                        Shapes::FCapsule capsuleA;
                        Shapes::FCapsule capsuleB;

                        bool inPath = false;

                        capsuleA.startPoint = closestCompPtr->m_CurrPos + offset;
                        capsuleA.endPoint   = closestCompPtr->m_NextPos + offset;
                        capsuleA.radius     = m_SplineFallRadius;

                        capsuleB.startPoint = closestCompPtr->m_CurrPos + offset;
                        capsuleB.endPoint   = closestCompPtr->m_PrevPos + offset;
                        capsuleB.radius     = m_SplineFallRadius;

                        inPath |= CollisionLibary::PointInCapsule(playerTransform->transform.translation, capsuleA);
                        inPath |= CollisionLibary::PointInCapsule(playerTransform->transform.translation, capsuleB);

                        if (inPath)
                                playerController->SetUseGravity(false);
                        else
                        {
                                playerController->SetUseGravity(true);
                                closestCompPtr = nullptr;
                        }
                }
                else
                {
                        playerController->SetUseGravity(true);
                }

                if (playerController->GetUseGravity() == false)
                {
                        XMVECTOR dirNext =
                            XMVector3Normalize(closestComp.m_NextPos + offset - playerTransform->transform.translation);
                        XMVECTOR dirPrev =
                            XMVector3Normalize(closestComp.m_PrevPos + offset - playerTransform->transform.translation);

                        XMVECTOR dir, next;
                        bool     attached = false;
                        if (MathLibrary::VectorDotProduct(playerTransform->transform.GetForward(), dirNext) > 0.3f)
                        {
                                attached = true;
                                next     = closestComp.m_NextPos + offset;
                        }
                        else if (MathLibrary::VectorDotProduct(playerTransform->transform.GetForward(), dirPrev) > 0.3f)
                        {
                                attached = true;
                                next     = closestComp.m_PrevPos + offset;
                        }

                        if (attached)
                        {
                                XMVECTOR closestPointOnLine = MathLibrary::GetClosestPointFromLineClamped(
                                    closestComp.m_CurrPos + offset, next, playerTransform->transform.translation);
                                float distanceToLine =
                                    MathLibrary::CalulateDistance(closestPointOnLine, playerTransform->transform.translation);

                                dir = XMVector3Normalize(next - playerTransform->transform.translation);

                                XMVECTOR worldInput = XMVector3Normalize(XMVector3Rotate(
                                    playerController->GetCurrentInput(), playerTransform->transform.rotation.data));

                                float inputDot = MathLibrary::VectorDotProduct(worldInput, dir);


                                inputDot = std::max(inputDot, 0.0f);

                                float forceAlpha = std::min(1.0f, distanceToLine / m_SplineFallRadius);
                                float attractionForce =
                                    MathLibrary::lerp(m_SplineAttractionForceMin, m_SplineAttractionForceMax, forceAlpha);

                                float strength = inputDot * attractionForce;

                                XMVECTOR currVel    = playerController->GetCurrentVelocity();
                                float    velMag     = MathLibrary::CalulateVectorLength(currVel);
                                XMVECTOR desiredVel = dir * velMag;

                                currVel = MathLibrary::MoveTowards(currVel, desiredVel, strength);
                                playerController->SetCurrentVelocity(currVel);
                                playerController->SetNextForward(XMVector3Normalize(next - closestComp.m_CurrPos));
                                playerController->SetCurrentMaxSpeed(6.0f);
                                // playerController->AddCurrentVelocity(dir * 10.0f * deltaTime);
                        }
                }
        }

        for (auto& it : m_SplineClusterSpawners)
        {
                if (it.second.current <= it.second.segments)
                {
                        if (it.second.spawnTimer <= 0.0f)
                        {
                                it.second.spawnTimer = it.second.spawnCD;
                                bool head            = it.second.current <= 1;
                                bool tail            = it.second.current == it.second.segments;

                                SpawnSplineOrb(it.second, it.first, tail, head);
                        }
                        else
                        {
                                it.second.spawnTimer -= deltaTime;
                        }
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
        m_HandleManager   = GEngine::Get()->GetHandleManager();
        m_SystemManager   = GEngine::Get()->GetSystemManager();
        m_ResourceManager = GEngine::Get()->GetResourceManager();


        auto baseMatHandle = m_ResourceManager->LoadMaterial("GlowSpeedboostBase");

        for (int i = 0; i < E_LIGHT_ORBS::COUNT; ++i)
        {
                auto handle = m_ResourceManager->CopyResource<Material>(baseMatHandle, speedboostMaterialNames[i].c_str());
                auto mat    = m_ResourceManager->GetResource<Material>(handle);
                speedboostMaterials[i] = handle;
                XMStoreFloat3(&mat->m_SurfaceProperties.emissiveColor,
                              2.0f * DirectX::PackedVector::XMLoadColor(&E_LIGHT_ORBS::ORB_COLORS[i]));
        }

        MazeGenerator mazeGenerator;
        m_Paths.resize(PathCount);
        for (int i = 0; i < PathCount; ++i)
        {
                m_Paths[i] = mazeGenerator.GeneratePath(15, 15, 8, 0, 8, 14);
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
