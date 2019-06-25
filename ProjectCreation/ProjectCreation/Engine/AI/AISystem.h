#pragma once

#include <DirectXMath.h>
#include <vector>
#include "../../ECS/ECS.h"
#include "AIComponent.h"

using namespace DirectX;

class AISystem : public ISystem
{
    private:
        HandleManager* m_HandleManager;
        SystemManager* m_SystemManager;

        XMVECTOR CalculateAlignment(AIComponent* boid);
        XMVECTOR CalculateCohesion(AIComponent* boid);
        XMVECTOR CalculateSeperation(AIComponent* boid);

        void CalculateAverage();

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
        float m_AlignmentStrength;
        float m_CohesionStrength;
        float m_SeperationStrength;

        XMVECTOR m_AveragePosition;
        XMVECTOR m_AverageForward;

		float m_FlockRadius;
};