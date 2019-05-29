#pragma once

#include "../../ECS/ECS.h"

class TransformComponent;

class SpeedBoostSystem : public ISystem
{
    private:
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;

        static constexpr uint32_t m_MaxSpeedBoosts = 20;
        ComponentHandle           m_BoostTransformHandles[m_MaxSpeedBoosts];


        void RandomMoveBoost(TransformComponent* boostTC, TransformComponent* playerTC);

        float m_TargetRadius = 0.0f;

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
        static constexpr float m_MaxBoostDistance = 7.0f;
        static constexpr float m_BoostRadius      = 0.03f;
};