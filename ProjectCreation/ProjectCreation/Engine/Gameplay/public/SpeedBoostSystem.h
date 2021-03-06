#pragma once
#include <DirectXMath.h>
#include <queue>
#include <vector>

#include <HandleManager.h>
#include <SystemManager.h>
#include <IResource.h>

#include <PlayerMovement.h>
#include <JGamePad.h>
#include <LightOrbColors.h>
#include <SpeedboostComponent.h>
#include <SplineCluster.h>

#include <Random.h>
class TransformComponent;
class ResourceManager;
class SpeedboostComponent;
class SpeedboostSplineComponent;
class PlayerController;

class SpeedBoostSystem : public ISystem
{
    private:
        HandleManager*   m_HandleManager;
        SystemManager*   m_SystemManager;
        ResourceManager* m_ResourceManager;

        static constexpr uint32_t m_MaxSpeedBoosts = 25;

        int m_ClusterCounter = 0;

        static constexpr int PathCount = 9;

        std::vector<std::vector<DirectX::XMVECTOR>> m_Paths;

        ResourceHandle speedboostMaterials[E_LIGHT_ORBS::COUNT]  = {};
        std::string speedboostMaterialNames[E_LIGHT_ORBS::COUNT] = {"SpeedboostR", "SpeedboostG", "SpeedboostB", "SpeedboostW"};
        std::string speedboostMeshNames[E_LIGHT_ORBS::COUNT]     = {"RedOrb00", "GreenOrb00", "BlueOrb00", "Sphere01"};

        // GW::AUDIO::GMusic* m_Spline_Ambience;

        // void RequestDestroySpeedboost(SpeedboostComponent* speedComp);
        void RequestDestroySplineOrb(SpeedboostSplineComponent* speedComp);

        ComponentHandle latchedSplineHandle;
        int             latchedSplineIndex = -1;

        bool m_EnableRandomSpawns = false;


        std::vector<float> x;

        void  RequestUnlatchFromSpline(PlayerController* playerController, float deltaTime);
        float mDelatchTimer = 0.0f;
        float mDelatchCD    = 0.5f;
        void  CreateRandomPath(const DirectX::XMVECTOR& start,
                               const DirectX::XMVECTOR& end,
                               int                      color,
                               float                    width     = 40.0f, // 50.0f
                               unsigned int             waveCount = 5,
                               float                    heightvar = 0.25f); // 1.6f

        void DestroySpline(int SplineID, int curr);

        float m_PlayerEffectRadius    = 200.0f;
        float m_SpawnBoostTimer       = 0.0f;
        float m_SpawnBoostCD          = 0.1f;
        float m_DestorySplineOrbCD    = 0.1f;
        float m_DestorySplineOrbTimer = 0.0f;
        float m_SplineSpawnCD         = 0.05f;
        float m_BoostLifespan         = 25.0f;
        float m_BoostLifespanVariance = 2.0f;
        float m_BoostShrinkSpeed      = m_BoostRadius;
        float m_targetTerrain         = 0.0f;

        float maxTutorialPathLength[4] = {25.0f, 25.0f, 25.0f, 25.0f};
        float maxPathLength[4]         = {30.0f, 60.0f, 90.0f, 100.0f};

        static constexpr float m_SplineLengthPerOrb       = 2.5f;
        static constexpr float m_SplineLatchRadius        = 0.2f;
        static constexpr float m_SplineFallRadius         = 0.3f;
        static constexpr float m_MaxSpawnDistance         = 16.0f;
        static constexpr float m_MinSpawnDistance         = 5.0f;
        static constexpr float m_SpawnAngle               = DirectX::XMConvertToRadians(110.0f);
        static constexpr float m_DespawnDistanceOffset    = 5.0f;
        static constexpr float m_BoostRadius              = 0.15f;
        static constexpr float m_PullSearchRadius         = 1.0f;
        static constexpr float m_SplineAttractionForceMin = 0.0f;
        static constexpr float m_SplineAttractionForceMax = 1.5f;
        static constexpr float m_SplineHeightOffset       = 0.06f;

        // TESTING
        int colorCount = 0;

    protected:
        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;


        uint8_t m_PendingPathCounts[4] = {0, 0, 0, 0};

    public:
        DirectX::XMVECTOR m_CurrentPathEnd;

        void RequestPath(int color);

        float splineHeight = 0.25f;
        float splineWidth  = 40.0f;

        std::unordered_map<int, SplineCluster> m_SplineClusterSpawners;

        bool        m_ColorsCollected[4] = {false, false, false, false};
        const char* spawnNames[3]        = {"redPlanetSpawn", "greenPlanetSpawn", "bluePlanetSpawn"};

        EntityHandle SpawnSpeedOrb();
        EntityHandle SpawnSplineOrb(SplineCluster& cluster, int clusterID, bool tail = false, bool head = false);

        EntityHandle SpawnLightOrb(const DirectX::XMVECTOR& pos, int color);

        inline void SetTargetTerrain(float val)
        {
                m_targetTerrain = val;
        };

        void UpdateSpeedboostEvents();

        void RequestDestroySpeedboost(SpeedboostComponent* speedComp);

        void RequestDestroyAllSpeedboosts();

        void RequestDestroyAllSplines();

        void RequestDestroyAllPlanets();

        inline void ResetLevel()
        {
                RequestDestroyAllSplines();
                RequestDestroyAllSpeedboosts();
                // RequestDestroyAllPlanets();
        }

        inline void SetRandomSpawnEnabled(bool val)
        {
                m_EnableRandomSpawns = val;
        };
};