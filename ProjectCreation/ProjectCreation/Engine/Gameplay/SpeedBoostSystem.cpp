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
#include "../MathLibrary/Splines/SplineUtility.h"

using namespace DirectX;

std::random_device                    r;
std::default_random_engine            e1(r());
std::uniform_real_distribution<float> uniform_dist(0.0f, 1.0f);


void SpeedBoostSystem::SpawnSpeedBoost(const FSpeedboostSpawnSettings& settings)
{
        ComponentHandle boostHandle;
        ComponentHandle transHandle;
        auto entityHandle = EntityFactory::CreateStaticMeshEntity("Sphere01", materialNames[settings.color], &boostHandle);
        boostHandle       = m_ComponentManager->AddComponent<SpeedboostComponent>(entityHandle);

        SpeedboostComponent* gsc = m_ComponentManager->GetComponent<SpeedboostComponent>(boostHandle);
        TransformComponent*  gtc = m_ComponentManager->GetComponent<TransformComponent>(entityHandle);

        float lifetime = settings.lifetime;
        if (settings.randomLifetime)
        {
                lifetime = MathLibrary::RandomFloatInRange(m_BoostLifespan - m_BoostLifespanVariance,
                                                           m_BoostLifespan + m_BoostLifespanVariance);
        }
        gsc->m_CanRespawn          = settings.randomLifetime;
        gtc->transform.translation = settings.pos;
        gtc->transform.SetScale(0.0f);
        gsc->m_CurrentRadius = 0.0f;
        gsc->m_Lifetime      = lifetime;
        gsc->m_Color         = settings.color;
        gsc->m_TargetRadius  = m_BoostRadius;
        gsc->m_WantsRespawn  = false;
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
        FGoodSpline spline(m_Paths[MathLibrary::GetRandomIntInRange(0, PathCount - 1)]);
        float       length   = spline.GetLength() * MathLibrary::CalulateDistance(start, end);
        int         segments = length / m_SplineLengthPerOrb;
        auto        points   = SplineUtility::BakePointsFromSplineAndTransform(spline, start, end, width, segments, waveCount);

        FSpeedboostSpawnSettings settings;
        settings.color          = color;
        settings.randomLifetime = false;
        m_SpawnQueue.push(settings);

        for (auto& p : points)
        {
                settings.pos = p;
                m_SpawnQueue.push(settings);
        }
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

                        float distSq = MathLibrary::CalulateDistanceSq(transComp->transform.translation,
                                                                       playerTransform->transform.translation);

                        bool onCD = false;

                        if (speedComp->m_Timer > 0)
                        {
                                onCD = true;
                                speedComp->m_Timer -= deltaTime;
                        }

                        float distanceSq = MathLibrary::CalulateDistanceSqIgnoreY(playerTransform->transform.translation,
                                                                                  transComp->transform.translation);

                        float checkRadius = m_BoostRadius * 4.0f;
                        if (!onCD && distanceSq < (checkRadius * checkRadius))
                        {

                                SYSTEM_MANAGER->GetSystem<ControllerSystem>()->IncreaseOrbCount(speedComp->m_Color);

                                // speedComp->m_WantsRespawn = true;
                                // speedComp->m_TargetRadius = 0.0f;
                                // m_PlayerEffectRadius += 1.0f;
                                speedComp->m_Timer = speedComp->m_CD;
                                static_cast<PlayerController*>(SYSTEM_MANAGER->GetSystem<ControllerSystem>()
                                                                   ->m_Controllers[ControllerSystem::E_CONTROLLERS::PLAYER])
                                    ->SpeedBoost(transComp->transform.translation);
                        }

                        TransformComponent* closestGoalTransform =
                            m_ComponentManager->GetComponent<TransformComponent>(closestGoalTransformHandle);

                        if (speedComp->m_CurrentRadius != speedComp->m_TargetRadius)
                        {
                                speedComp->m_CurrentRadius = MathLibrary::MoveTowards(
                                    speedComp->m_CurrentRadius, speedComp->m_TargetRadius, m_BoostShrinkSpeed * deltaTime);
                                transComp->transform.SetScale(speedComp->m_CurrentRadius);
                        }

                        if (speedComp->m_CanRespawn)
                        {
                                if (speedComp->m_Lifetime < 0.0f || distSq > (m_MaxBoostDistance) * (m_MaxBoostDistance))
                                {
                                        speedComp->m_WantsRespawn = true;
                                        speedComp->m_TargetRadius = 0.0f;
                                }

                                speedComp->m_Lifetime -= deltaTime;
                        }

                        if (speedComp->m_WantsRespawn)
                        {
                                if (speedComp->m_CurrentRadius <= 0.0f)
                                {
                                        XMVECTOR pos =
                                            MathLibrary::GetRandomPointInRadius2D(playerTransform->transform.translation,
                                                                                  m_MaxBoostDistance - 5.2f,
                                                                                  m_MaxBoostDistance - 0.8f);

                                        speedComp->m_Lifetime =
                                            MathLibrary::RandomFloatInRange(m_BoostLifespan - m_BoostLifespanVariance,
                                                                            m_BoostLifespan + m_BoostLifespanVariance);
                                        transComp->transform.translation = pos;
                                        speedComp->m_TargetRadius        = m_BoostRadius;
                                        speedComp->m_WantsRespawn        = false;
                                }
                        }
                }
        }

        TransformComponent* closestGoalTransform =
            m_ComponentManager->GetComponent<TransformComponent>(closestGoalTransformHandle);

        if (m_EnableRandomSpawns && randomOrbCount < m_MaxSpeedBoosts)
        {
                FSpeedboostSpawnSettings settings;
                XMVECTOR                 pos = MathLibrary::GetRandomPointInRadius2D(
                    playerTransform->transform.translation, m_MaxBoostDistance - 5.2f, m_MaxBoostDistance - 0.8f);
                settings.pos            = pos;
                settings.color          = MathLibrary::GetRandomIntInRange(0, E_LIGHT_ORBS::COUNT - 1);
                settings.randomLifetime = true;
                m_SpawnQueue.push(settings);
                randomOrbCount++;
        }

        if (m_SpawnBoostTimer <= 0)
        {
                m_SpawnBoostTimer = m_SpawnBoostCD;

                if (m_SpawnQueue.empty() == false)
                {
                        auto spawnSettings = m_SpawnQueue.front();
                        m_SpawnQueue.pop();

                        SpawnSpeedBoost(spawnSettings);
                }
        }
        else
        {
                m_SpawnBoostTimer -= deltaTime;
        }
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


        FSpeedboostSpawnSettings settings;
        settings.pos            = playerTransform->transform.translation + 2.0f * VectorConstants::Forward;
        settings.color          = E_LIGHT_ORBS::WHITE_LIGHTS;
        settings.randomLifetime = false;
        m_SpawnQueue.push(settings);

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
