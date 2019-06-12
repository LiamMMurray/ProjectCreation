#pragma once

#include "../../ECS/ECS.h"
#include "LightOrbColors.h"
#include <DirectXMath.h>
#include <vector>
class TransformComponent;
class SpeedboostComponent;

class SpeedBoostSystem : public ISystem
{
    private:
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;
        SystemManager*    m_SystemManager;

        static constexpr uint32_t m_MaxSpeedBoosts = 10;
        int                       currentOrbCounter[E_LIGHT_ORBS::COUNT]{};

		static constexpr int PathCount = 9;

        std::vector<std::vector<DirectX::XMVECTOR>> m_Paths;

        void RespawnSpeedBoost(TransformComponent*       boostTC,
                               SpeedboostComponent*      boostSC,
                               const TransformComponent* playerTC,
                               const TransformComponent* targetTC);

        const char* materialNames[4] = {"GlowSpeedboost01", "GlowSpeedboost02", "GlowSpeedboost03", "GlowSpeedboost04"};

        void SpawnSpeedBoost(DirectX::XMVECTOR position, int color);

        float m_PlayerEffectRadius    = 0.0f;
        float m_SpawnBoostTimer       = 0.0f;
        float m_SpawnBoostCD          = 2.0f;
        float m_BoostLifespan         = 8.0f;
        float m_BoostLifespanVariance = 2.0f;
        float m_BoostShrinkSpeed      = m_BoostRadius;

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