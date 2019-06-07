#pragma once

#include "../../ECS/ECS.h"
#include "..//MathLibrary/MathLibrary.h"

class TransformComponent;

class OrbitSystem : public ISystem
{
    private:
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;

        const char* materialNames[3]       = {"GlowMatPlanet01", "GlowMatPlanet02", "GlowMatPlanet03"};
        int         m_Stage                = 0;
        void        CreateGoal(int n);

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