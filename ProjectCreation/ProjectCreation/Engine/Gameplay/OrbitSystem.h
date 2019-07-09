#pragma once

#include "../../ECS/HandleManager.h"
#include "../../ECS/SystemManager.h"
#include "../MathLibrary/MathLibrary.h"

class TransformComponent;

class OrbitSystem : public ISystem
{
    private:
        HandleManager* m_HandleManager;

        const char* materialNames[3]       = {"GlowMatPlanet01", "GlowMatPlanet02", "GlowMatPlanet03"};
        int         m_Stage                = 0;
        void        CreateGoal(int color, DirectX::XMVECTOR position);

        DirectX::XMVECTORF32 orbitCenter = {0.0f, 1000.0f, 0.0f, 1.0f};
        ComponentHandle      m_ClosestGoalTransform;

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
        inline ComponentHandle GetClosestGoalTransform() const
        {
                return m_ClosestGoalTransform;
        }

        static constexpr DirectX::XMVECTORF32 GoalPositions[3] = {{0.0f, 0.0f, 100.0f, 1.0f},
                                                                  {-100.0f, 0.0f, -11.0f, 1.0f},
                                                                  {100.0f, 0.0f, -11.0f, 1.0f}};
};