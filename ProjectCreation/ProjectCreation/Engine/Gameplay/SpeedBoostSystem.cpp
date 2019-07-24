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

#include <limits>
#include "..//CollisionLibary/CollisionLibary.h"
#include "../AI/AIComponent.h"

using namespace DirectX;

std::random_device                    r;
std::default_random_engine            e1(r());
std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);


EntityHandle SpeedBoostSystem::SpawnSpeedOrb()
{
        EntityHandle controlledEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                            ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
                                            ->GetControlledEntity();

        TransformComponent* playerTransform = controlledEntity.GetComponent<TransformComponent>();
        XMFLOAT3            euler           = playerTransform->transform.rotation.ToEulerAngles();
        euler.x                             = 0.0f;
        FQuaternion quat                    = FQuaternion::FromEulerAngles(euler);
        int         color                   = MathLibrary::GetRandomIntInRange(0, 4);
        XMVECTOR    pos                     = MathLibrary::GetRandomPointInArc(playerTransform->transform.translation,
                                                        quat.GetForward(),
                                                        VectorConstants::Up,
                                                        m_SpawnAngle,
                                                        m_MinSpawnDistance,
                                                        m_MaxSpawnDistance);
        pos                                 = XMVectorSetY(pos, 0.0f);
        auto entityH                        = SpawnLightOrb(pos, color);
        auto speedBoostHandle               = entityH.AddComponent<SpeedboostComponent>();
        entityH.AddComponent<AIComponent>();
        auto speedboostComponent             = speedBoostHandle.Get<SpeedboostComponent>();
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

        // cluster.color = MathLibrary::GetRandomIntInRange(0, E_LIGHT_ORBS::COUNT);

        if (changeColor)
        {
                colorCount++;

                if (colorCount < 5)
                {
                        cluster.color = E_LIGHT_ORBS::RED_LIGHTS;
                }

                else if (colorCount >= 5 && colorCount < 10)
                {
                        cluster.color = E_LIGHT_ORBS::BLUE_LIGHTS;
                }

                else if (colorCount >= 10 && colorCount < 15)
                {
                        cluster.color = E_LIGHT_ORBS::GREEN_LIGHTS;
                }

                else
                {
                        colorCount = 0;
                }
        }

        cluster.cachedPoints.push_back(curr);
        cluster.cachedColors.push_back(cluster.color);
        auto entityH = SpawnLightOrb(curr, cluster.color);
        auto splineH = entityH.AddComponent<SpeedboostSplineComponent>();

        cluster.splineComponentList.push_back((splineH));
        auto splineComp       = splineH.Get<SpeedboostSplineComponent>();
        splineComp->bHead     = head;
        splineComp->bTail     = tail;
        splineComp->color     = cluster.color;
        splineComp->index     = cluster.current - 1;
        splineComp->clusterID = clusterID;

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

void SpeedBoostSystem::RequestDestroyAllSpeedboosts()
{
        for (auto& iter : m_HandleManager->GetActiveComponents<SpeedboostComponent>())
        {
                RequestDestroySpeedboost(&iter);
        }
}

void SpeedBoostSystem::RequestDestroySplineOrb(SpeedboostSplineComponent* speedComp)
{
        auto orbComp            = speedComp->GetParent().GetComponent<OrbComponent>();
        orbComp->m_WantsDestroy = true;
        speedComp->GetHandle().SetIsActive(false);

        SpeedboostSplineComponent* latchedSpline = latchedSplineHandle.Get<SpeedboostSplineComponent>();

        if (latchedSpline == speedComp)
        {
                bIsLatchedToSpline = false;
        }

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
                                // m_EnableRandomSpawns = true;
                                bFirstStage = true;
                        }
                }
        }

        auto orbitSystem = SYSTEM_MANAGER->GetSystem<OrbitSystem>();

        XMVECTOR start = XMVectorZero();


        static bool goals[4] = {};
        {
                for (int i = 0; i < 3; ++i)
                {
                        if (!goals[i])
                        {
                                int count = controllerSystem->GetOrbCount(i);
                                if (count >= 3)
                                {
                                        start = playerTransform->transform.translation +
                                                2.0f * playerTransform->transform.GetForward();

                                        XMVECTOR dir = orbitSystem->GoalPositions[i] - start;

                                        XMVECTOR end = orbitSystem->GoalPositions[i] - 2.0f * XMVector3Normalize(dir);

                                        CreateRandomPath(start, end, i, splineWidth, 5, splineHeight);
										
                                        // XMVECTOR randStart = MathLibrary::GetRandomPointInRadius2D(XMVectorZero(), 0, 50);
                                        // XMVECTOR randEnd   = MathLibrary::GetRandomPointInRadius2D(XMVectorZero(), -50, 0);
                                        //
                                        // CreateRandomPath(randStart, end, i);
                                        //
                                        // randStart = MathLibrary::GetRandomPointInRadius2D(XMVectorZero(), 0, 50);
                                        // randEnd   = MathLibrary::GetRandomPointInRadius2D(XMVectorZero(), -50, 0);
                                        //
                                        // CreateRandomPath(randStart, end, i);

                                        goals[i] = true;
                                }
                        }
                }
        }
}

void SpeedBoostSystem::RequestUnlatchFromSpline(PlayerController* playerController, float deltaTime)
{
        if (mDelatchTimer > 0.0f)
        {
                mDelatchTimer -= deltaTime;
        }
        else
        {
                playerController->SetUseGravity(true);

                if (bIsLatchedToSpline)
                {
                        bIsLatchedToSpline                           = false;
                        SpeedboostSplineComponent* latchedSplineComp = latchedSplineHandle.Get<SpeedboostSplineComponent>();
                        TransformComponent*        playerTransform =
                            playerController->GetControlledEntity().GetComponent<TransformComponent>();
                        // Destroy the old spline
                        return;

                        DestroySpline(latchedSplineComp->clusterID, latchedSplineComp->index);

                        int splineID  = latchedSplineComp->clusterID;
                        int goalIndex = m_SplineClusterSpawners.at(splineID).color;

                        ControllerSystem* controllerSystem = SYSTEM_MANAGER->GetSystem<ControllerSystem>();

                        //controllerSystem->ResetOrbCount(goalIndex);

                        // controllerSystem->m_OrbCounts = 0;

                        // Start of the new spline
                        XMVECTOR start =
                            playerTransform->transform.translation + 2.0f * playerTransform->transform.GetForward();
                        
                        // Direction of the new spline
                        XMVECTOR dir = SYSTEM_MANAGER->GetSystem<OrbitSystem>()->GoalPositions[goalIndex] - start;
                        
                        // End point of the new spline
                        XMVECTOR end =
                            SYSTEM_MANAGER->GetSystem<OrbitSystem>()->GoalPositions[goalIndex] - 2.0f *
                            XMVector3Normalize(dir);
                        
                        // Finish creation of new spline
                        CreateRandomPath(start, end, goalIndex);
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
        transform.translation          = start;
        transform.rotation             = FQuaternion::LookAt(XMVector3Normalize(dir));
        transform.scale                = scale;
        it.first->second.transform     = transform.CreateMatrix();
        it.first->second.segments      = length / m_SplineLengthPerOrb;
        it.first->second.spiralCount   = waveCount;
        it.first->second.maxHeight     = heightvar;
        it.first->second.color         = color;
        it.first->second.targetColor   = color;
        it.first->second.current       = 0;
        it.first->second.shouldDestroy = false;
        it.first->second.spawnTimer = it.first->second.spawnCD = m_SplineSpawnCD;
        XMVECTOR dummy;
        it.first->second.BakeNextPointOnSpline(dummy, dummy, dummy);
        it.first->second.current = 0;


        m_ClusterCounter++;
}

void SpeedBoostSystem::DestroySpline(int SplineID, int start)
{
        SplineCluster& toDelete = m_SplineClusterSpawners.at(SplineID);
        toDelete.shouldDestroy  = true;
        toDelete.current        = start;
        toDelete.deleteIndex    = 0;
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

        static float targetTerrain = 0.0f;

        targetTerrain = m_targetTerrain;

        GEngine::Get()->m_TerrainAlpha = MathLibrary::lerp(GEngine::Get()->m_TerrainAlpha, targetTerrain, deltaTime * 0.1f);

        if (GCoreInput::GetKeyState(KeyCode::T) == KeyState::DownFirst)
        {
                SetTargetTerrain(1.0f);
                targetTerrain = m_targetTerrain;
        }

        // Debug Testing CHEAT to spawn a white spline that will carry player to origin
        if (GCoreInput::GetKeyState(KeyCode::J) == KeyState::DownFirst)
        {
                XMVECTOR start = playerTransform->transform.translation + 2.0f * playerTransform->transform.GetForward();

                // Direction of the new spline
                XMVECTOR dir = XMVectorZero() - start;

                // End point of the new spline
                XMVECTOR end = XMVectorZero() - 2.0f * XMVector3Normalize(dir);

                CreateRandomPath(start, end, 3);
        }


        // GEngine::Get()->m_PlayerRadius = MathLibrary::lerp(GEngine::Get()->m_PlayerRadius, m_PlayerEffectRadius, deltaTime);

        XMVECTOR flatPlayerForward;
        {
                XMFLOAT3 euler    = playerTransform->transform.rotation.ToEulerAngles();
                euler.x           = 0.0f;
                FQuaternion quat  = FQuaternion::FromEulerAngles(euler);
                flatPlayerForward = quat.GetForward();
        }

        // m_PlayerEffectRadius                       = 25.0f;
        int speedboostCount = 0;
        {
                for (auto& speedComp : m_HandleManager->GetActiveComponents<SpeedboostComponent>())
                {
                        XMVECTOR center = speedComp.GetParent().GetComponent<TransformComponent>()->transform.translation;

                        speedboostCount++;

                        // START: Move speed boosts with ai stuff here

                        // END: Move speed boosts with ai stuff here
                        XMVECTOR dir        = center - playerTransform->transform.translation;
                        float    distanceSq = MathLibrary::VectorDotProduct(dir, dir);

                        float checkRadius = speedComp.collisionRadius;

                        if (speedComp.lifetime > 0.0f && distanceSq < (checkRadius * checkRadius))
                        {
                                if (playerController->SpeedBoost(center, speedComp.color))
                                {
                                        // RequestDestroySpeedboost(&speedComp);
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

                        if (distanceSq < (checkRadius * checkRadius))
                        {
                                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->IncreaseOrbCount(speedComp.color);
                                playerController->SpeedBoost(center, speedComp.color);
                                RequestDestroySpeedboost(&speedComp);
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

                float checkRadius = m_SplineLatchRadius;
                bool  foundFriend = false;
                float prevDistance;
                float prevDot;

                if (bIsLatchedToSpline == true)
                {
                        SpeedboostSplineComponent* closestSplineComp = latchedSplineHandle.Get<SpeedboostSplineComponent>();
                        TransformComponent* transComp = closestSplineComp->GetParent().GetComponent<TransformComponent>();
                        XMVECTOR prevVector = transComp->transform.translation - playerTransform->transform.translation;
                        prevDistance        = MathLibrary::CalulateVectorLength(prevVector);
                        prevVector          = XMVector3Normalize(prevVector);
                        prevDot = MathLibrary::VectorDotProduct(prevVector, playerTransform->transform.GetForward());
                }
                else
                {
                        prevDistance = INFINITY;
                        prevDot      = -1.0f;
                }

                bool shouldLatch = false;


                for (auto& splineComp : m_HandleManager->GetActiveComponents<SpeedboostSplineComponent>())
                {
                        int index     = splineComp.index;
                        int clusterID = splineComp.clusterID;

                        auto clusterIt = m_SplineClusterSpawners.find(clusterID);


                        XMVECTOR pos = splineComp.GetParent().GetComponent<TransformComponent>()->transform.translation;
                        clusterIt->second.cachedPoints[index] = pos;
                        clusterIt->second.cachedColors[index] = splineComp.color;
                        int splineColor                       = splineComp.color;

                        XMVECTOR dirVector = pos - playerTransform->transform.translation;
                        float    distance  = MathLibrary::CalulateVectorLength(dirVector);
                        dirVector          = XMVector3Normalize(dirVector);
                        float dot          = MathLibrary::VectorDotProduct(dirVector, playerTransform->transform.GetForward());

                        if (distance < (checkRadius))
                        {
                                if ((bIsLatchedToSpline == false || dot > prevDot))
                                {
                                        latchedSplineHandle = splineComp.GetHandle();
                                        shouldLatch         = true;
                                }
                        }
                }

                if (bIsLatchedToSpline || shouldLatch)
                {
                        SpeedboostSplineComponent* latchedSplineComp = latchedSplineHandle.Get<SpeedboostSplineComponent>();
                        int                        index             = latchedSplineComp->index;
                        int                        clusterID         = latchedSplineComp->clusterID;

                        auto clusterIt = m_SplineClusterSpawners.find(clusterID);

                        if (clusterIt == m_SplineClusterSpawners.end())
                        {
                                bIsLatchedToSpline = false;
                        }
                        else
                        {
                                XMVECTOR currPos = clusterIt->second.cachedPoints[index];
                                XMVECTOR nextPos;
                                XMVECTOR prevPos;

                                if (index <= 0)
                                {
                                        prevPos = currPos;
                                }
                                else
                                {
                                        prevPos = clusterIt->second.cachedPoints[index - 1];
                                }

                                if (index >= clusterIt->second.cachedPoints.size() - 1)
                                {
                                        nextPos = currPos;
                                }
                                else
                                {
                                        nextPos = clusterIt->second.cachedPoints[index + 1];
                                }


                                Shapes::FCapsule capsuleA;
                                Shapes::FCapsule capsuleB;

                                bool inPath = false;

                                capsuleA.startPoint = currPos + offset;
                                capsuleA.endPoint   = nextPos + offset;
                                capsuleA.radius     = m_SplineFallRadius;

                                capsuleB.startPoint = capsuleA.startPoint;
                                capsuleB.endPoint   = prevPos + offset;
                                capsuleB.radius     = m_SplineFallRadius;

                                inPath |= CollisionLibary::PointInCapsule(playerTransform->transform.translation, capsuleA);
                                inPath |= CollisionLibary::PointInCapsule(playerTransform->transform.translation, capsuleB);

                                int correctColor = latchedSplineComp->color;

                                for (int i = 0; i < E_LIGHT_ORBS::WHITE_LIGHTS; ++i)
                                {
                                        if (i == correctColor)
                                        {
                                                inPath &= (GCoreInput::GetKeyState(playerController->m_ColorInputKeyCodes[i]) ==
                                                           KeyState::Down);
                                        }
                                        else
                                        {
                                                inPath &= ~(GCoreInput::GetKeyState(
                                                                playerController->m_ColorInputKeyCodes[i]) == KeyState::Down);
                                        }
                                }

                                if (inPath)
                                {
                                        bIsLatchedToSpline = true;
                                        mDelatchTimer      = mDelatchCD;
                                        playerController->SetUseGravity(false);

                                        XMVECTOR dirNext = XMVector3Normalize(nextPos - currPos);
                                        XMVECTOR dirPrev = XMVector3Normalize(prevPos - currPos);

                                        XMVECTOR dirVel;
                                        XMVECTOR dir;
                                        bool     attached = false;
                                        if (MathLibrary::VectorDotProduct(playerTransform->transform.GetForward(), dirNext) >
                                            0.3f)
                                        {
                                                attached = true;
                                                dir      = dirNext;
                                                dirVel   = XMVector3Normalize(capsuleA.endPoint -
                                                                            playerTransform->transform.translation);
                                        }
                                        else if (MathLibrary::VectorDotProduct(playerTransform->transform.GetForward(),
                                                                               dirPrev) > 0.3f)
                                        {
                                                attached = true;
                                                dir      = dirPrev;
                                                dirVel   = XMVector3Normalize(capsuleB.endPoint -
                                                                            playerTransform->transform.translation);
                                        }

                                        if (attached)
                                        {
                                                XMVECTOR closestPointOnLine = MathLibrary::GetClosestPointFromLine(
                                                    capsuleA.startPoint,
                                                    capsuleA.startPoint + dirVel,
                                                    playerTransform->transform.translation);
                                                float distanceToLine = MathLibrary::CalulateDistance(
                                                    closestPointOnLine, playerTransform->transform.translation);

                                                XMVECTOR worldInput = XMVector3Normalize(
                                                    XMVector3Rotate(playerController->GetCurrentInput(),
                                                                    playerTransform->transform.rotation.data));

                                                float inputDot = MathLibrary::VectorDotProduct(worldInput, dirVel);


                                                inputDot = std::max(inputDot, 0.0f);

                                                float forceAlpha      = std::min(1.0f, distanceToLine / m_SplineFallRadius);
                                                float attractionForce = MathLibrary::lerp(
                                                    m_SplineAttractionForceMin, m_SplineAttractionForceMax, forceAlpha);

                                                float strength = inputDot * attractionForce;

                                                XMVECTOR currVel    = playerController->GetCurrentVelocity();
                                                float    velMag     = MathLibrary::CalulateVectorLength(currVel);
                                                XMVECTOR desiredVel = dirVel * velMag;

                                                currVel = MathLibrary::MoveTowards(currVel, desiredVel, strength);
                                                playerController->SetCurrentVelocity(currVel);
                                                playerController->SetNextForward(dir);
                                                playerController->SetCurrentMaxSpeed(6.0f);
                                                // playerController->AddCurrentVelocity(dir * 10.0f * deltaTime);
                                        }
                                }

                                else
                                {
                                        // Player has fallen off the spline
                                        RequestUnlatchFromSpline(playerController, deltaTime);
                                }
                        }
                }
                else
                {
                        RequestUnlatchFromSpline(playerController, deltaTime);
                }
        }

        for (auto it = m_SplineClusterSpawners.begin(); it != m_SplineClusterSpawners.end();)
        {
                if (it->second.shouldDestroy == false)
                {
                        if (it->second.current <= it->second.segments)
                        {
                                if (it->second.spawnTimer <= 0.0f)
                                {
                                        it->second.spawnTimer = it->second.spawnCD;
                                        bool head             = it->second.current <= 1;
                                        bool tail             = it->second.current == it->second.segments;

                                        SpawnSplineOrb(it->second, it->first, tail, head);
                                }
                                else
                                {
                                        it->second.spawnTimer -= deltaTime;
                                }
                        }
                }
                else
                {
                        size_t size = it->second.splineComponentList.size();
                        int    next = it->second.current + it->second.deleteIndex;
                        int    prev = it->second.current - it->second.deleteIndex;

                        bool done = true;

                        if (next < size)
                        {
                                SpeedboostSplineComponent* comp =
                                    it->second.splineComponentList[next].Get<SpeedboostSplineComponent>();
                                RequestDestroySplineOrb(comp);
                                done = false;
                        }

                        if (prev >= 0 && prev != next)
                        {
                                SpeedboostSplineComponent* comp =
                                    it->second.splineComponentList[prev].Get<SpeedboostSplineComponent>();
                                RequestDestroySplineOrb(comp);
                                done = false;
                        }

                        it->second.deleteIndex++;
                        if (done)
                        {
                                int id = it->first;
                                ++it;
                                m_SplineClusterSpawners.erase(id);
                                continue;
                        }
                }
                ++it;
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
        m_HandleManager    = GEngine::Get()->GetHandleManager();
        m_SystemManager    = GEngine::Get()->GetSystemManager();
        m_ResourceManager  = GEngine::Get()->GetResourceManager();
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

        // EntityHandle playerEntity = SYSTEM_MANAGER->GetSystem<ControllerSystem>()
        //                                 ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER]
        //                                 ->GetControlledEntity();
        //
        // TransformComponent* playerTransform = playerEntity.GetComponent<TransformComponent>();
        //
        // XMVECTOR pos    = playerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        // auto     handle = SpawnLightOrb(pos, E_LIGHT_ORBS::WHITE_LIGHTS);
        //
        // auto speedboostComponent             = handle.AddComponent<SpeedboostComponent>().Get<SpeedboostComponent>();
        // speedboostComponent->collisionRadius = m_BoostRadius;
        // speedboostComponent->lifetime        = 1.0f;
        // speedboostComponent->decay           = 0.0f;
        // speedboostComponent->color           = E_LIGHT_ORBS::WHITE_LIGHTS;
        /*CreateRandomPath(playerTransform->transform.translation,
                         SYSTEM_MANAGER->GetSystem<OrbitSystem>()->GoalPositions[0],
                         E_LIGHT_ORBS::BLUE_LIGHTS,
                         5.0f,
                         4);*/
        bIsLatchedToSpline = false;
}

void SpeedBoostSystem::OnShutdown()
{}

void SpeedBoostSystem::OnResume()
{}

void SpeedBoostSystem::OnSuspend()
{}
