#pragma once

#include "../../ECS/ECS.h"

class TransformComponent;

class SpeedBoostSystem : public ISystem
{
    private:
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;

        static const uint32_t m_MaxSpeedBoosts = 8;
        ComponentHandle       m_BoostTransformHandles[m_MaxSpeedBoosts];

        float m_MaxBoostDistance = 10.0f;
        float m_BoostRadius = 0.03f;

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
};