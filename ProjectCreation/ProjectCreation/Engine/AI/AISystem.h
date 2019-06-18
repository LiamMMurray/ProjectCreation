#pragma once

#include <DirectXMath.h>
#include <vector>
#include "../../ECS/ECS.h"

using namespace DirectX;

class AIComponent;

class AISystem : public ISystem
{
    private:
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;
        SystemManager*    m_SystemManager;

        XMVECTOR calculateAlignment(AIComponent* boid);
        XMVECTOR calculateCohesion(AIComponent* boid);
        XMVECTOR calculateSeperation(AIComponent* boid);

        void calculateAverage();

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
        std::vector<AIComponent*> m_boidVector;

        float boidAlignmentStrength  = 0.0f;
        float boidCohesionStrength   = 0.0f;
        float boidSeperationStrength = 0.0f;
};