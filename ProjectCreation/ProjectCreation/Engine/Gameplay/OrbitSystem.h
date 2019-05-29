#pragma once

#include "../../ECS/ECS.h"
#include "..//MathLibrary/MathLibrary.h"

class TransformComponent;

class OrbitSystem : public ISystem
{
    private:
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;

		ComponentHandle m_PlanetOutlines[3];
		ComponentHandle m_PlanetGoals[3];
        ComponentHandle* m_TargetTransforms[3];

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