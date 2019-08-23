#pragma once

#include "../../ECS/HandleManager.h"
#include "../../ECS/SystemManager.h"
#include "..//ResourceManager/IResource.h"
#include "../MathLibrary/MathLibrary.h"
#include "GoalComponent.h"

class TransformComponent;
class ResourceManager;
class PlayerController;

class OrbitSystem : public ISystem
{
    private:
        HandleManager* m_HandleManager;

        ResourceManager* m_ResourceManager;

        PlayerController* m_PlayerController;

        const char* materialNames[4] = {"GlowMatPlanet01", "GlowMatPlanet03", "GlowMatPlanet02", "GlowMatSun"};
        int         m_Stage          = 0;
        void        CreateGoal(int color, DirectX::XMVECTOR position);
        void        CreateTutorialGoal(int color, DirectX::XMVECTOR position);

        float             orbitOffset = 1300.0f;
        FQuaternion       sunRotation;
        DirectX::XMVECTOR orbitCenter;

        struct FActiveGoalInfo
        {
                EntityHandle activeGoalGround;
                EntityHandle activeGoalOrbit;
                int          activeColor;
                bool         hasActiveGoal = false;
        };

        static constexpr float goalDistances[4] = {10.0f, 10.0f, 10.0f, 10.0f};

        std::vector<ComponentHandle> sunAlignedTransforms;


        void UpdateSunAlignedObjects();

    protected:
        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;

        bool tutorialPlanets[4] = {false, false, false, false};

    public:
        int                          collectEventTimestamps[4] = {-1, -1, -1, -1};
        double                       PlanetPickupTimestamps[3] = {-1, -1, -1};
        std::vector<ComponentHandle> sunAlignedTransformsSpawning;

        void InstantCreateOrbitSystem();

        void InstantInOrbit(int color);
        void InstantRemoveFromOrbit();

        unsigned int    goalsCollected   = 0;
        bool            collectedMask[3] = {};
        FActiveGoalInfo activeGoal;
        void            DestroyPlanet(GoalComponent* toDestroy);
        ComponentHandle sunHandle, ring1Handle, ring2Handle, ring3Handle;
};