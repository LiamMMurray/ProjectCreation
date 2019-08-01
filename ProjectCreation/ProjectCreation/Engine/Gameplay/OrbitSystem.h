#pragma once

#include "../../ECS/HandleManager.h"
#include "../../ECS/SystemManager.h"
#include "../MathLibrary/MathLibrary.h"
#include "GoalComponent.h"

class TransformComponent;

class OrbitSystem : public ISystem
{
    private:
        HandleManager* m_HandleManager;

        const char* materialNames[3] = {"GlowMatPlanet01", "GlowMatPlanet03", "GlowMatPlanet02"};
        const char* spawnNames[3]    = {"redPlanetSpawn", "greenPlanetSpawn", "bluePlanetSpawn"};
        int         m_Stage          = 0;
        void        CreateGoal(int color, DirectX::XMVECTOR position);

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

        static constexpr float goalDistances[3] = {65.0f, 65.0f, 65.0f};

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

        int collectEventTimestamps[3] = {-1, -1, -1};

    public:
        unsigned int    goalsCollected   = 0;
        bool            collectedMask[3] = {};
        FActiveGoalInfo activeGoal;
        void            DestroyPlanet(GoalComponent* toDestroy);
};