#pragma once

#include <DirectXMath.h>
#include <queue>
#include <vector>
#include "../../ECS/ECS.h"
#include "LightOrbColors.h"
#include "SpeedboostComponent.h"
#include "SplineCluster.h"

class TransformComponent;
class SpeedboostComponent;

class SpeedBoostSystem : public ISystem
{
    private:
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;
        SystemManager*    m_SystemManager;

        static constexpr uint32_t m_MaxSpeedBoosts = 30;
        unsigned int              randomOrbCount   = 0;

		float                      flyTimer = 0.0f;
		float                      flyCD = 1.0f;
        std::vector<SplineCluster> m_SplineClusterSpawners;

        static constexpr int PathCount = 9;

        std::vector<std::vector<DirectX::XMVECTOR>> m_Paths;

        const char* materialNames[4] = {"GlowSpeedboost02", "GlowSpeedboost01", "GlowSpeedboost03", "GlowSpeedboost04"};

        void         SpawnRandomSpeedBoost();
        void         SpawnSplineSpeedBoost(const SplineCluster& cluster,
                                           unsigned int         index,
                                           int                  color,
                                           bool                 tail = false,
                                           bool                 head = false);
        EntityHandle SpawnSpeedBoost(const DirectX::XMVECTOR& pos, int color);

        float m_PlayerEffectRadius    = 0.0f;
        float m_SpawnBoostTimer       = 0.0f;
        float m_SpawnBoostCD          = 0.2f;
        float m_BoostLifespan         = 8.0f;
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
        static constexpr float m_MaxBoostDistance = 5.0f;
        static constexpr float m_BoostRadius      = 0.03f;
};