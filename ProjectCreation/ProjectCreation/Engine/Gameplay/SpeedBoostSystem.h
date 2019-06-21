#pragma once


#include <DirectXMath.h>
#include <queue>
#include <vector>

#include "../../ECS/HandleManager.h"
#include "../../ECS/SystemManager.h"

#include "LightOrbColors.h"
#include "SpeedboostComponent.h"
#include "SplineCluster.h"

class TransformComponent;
class SpeedboostComponent;

class SpeedBoostSystem : public ISystem
{
    private:
        HandleManager* m_HandleManager;
        SystemManager* m_SystemManager;

        static constexpr uint32_t m_MaxSpeedBoosts = 30;

        float                                  flyTimer = 0.0f;
        float                                  flyCD    = 2.0f;
        std::unordered_map<int, SplineCluster> m_SplineClusterSpawners;
        int                                    m_ClusterCounter = 0;

        static constexpr int PathCount = 9;

        std::vector<std::vector<DirectX::XMVECTOR>> m_Paths;

        const char* materialNames[4] = {"GlowSpeedboost02", "GlowSpeedboost01", "GlowSpeedboost03", "GlowSpeedboost04"};

        EntityHandle SpawnSpeedOrb();
        EntityHandle SpawnSplineOrb(const SplineCluster& cluster, int clusterID, bool tail = false, bool head = false);
        EntityHandle SpawnLightOrb(const DirectX::XMVECTOR& pos, int color);

        void RequestDestroySpeedboost(SpeedboostComponent* speedComp);

        float m_PlayerEffectRadius    = 0.0f;
        float m_SpawnBoostTimer       = 0.0f;
        float m_SpawnBoostCD          = 0.2f;
        float m_BoostLifespan         = 25.0f;
        float m_BoostLifespanVariance = 2.0f;
        float m_BoostShrinkSpeed      = m_BoostRadius;
        float m_SplineLengthPerOrb    = 0.8f;

        bool m_EnableRandomSpawns = false;

        void               UpdateSpeedboostEvents();
        std::vector<float> x;

        void CreateRandomPath(const DirectX::XMVECTOR& start,
                              const DirectX::XMVECTOR& end,
                              int                      color,
                              float                    width     = 3.0f,
                              unsigned int             waveCount = 2,
                              float                    heightvar = 0.3f);

    protected:
        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;

    public:
        static constexpr float m_MaxSpawnDistance = 5.0f;
        static constexpr float m_MinSpawnDistance = 1.0f;
        static constexpr float m_SpawnAngle       = DirectX::XMConvertToRadians(110.0f);
        static constexpr float m_DespawnDistance  = 6.5f;
        static constexpr float m_BoostRadius      = 0.1f;
        static constexpr float m_PullSearchRadius = 1.0f;
};