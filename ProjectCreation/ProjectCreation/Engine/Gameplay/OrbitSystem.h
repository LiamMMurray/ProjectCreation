#pragma once

#include <functional>
#include <vector>

#include <HandleManager.h>
#include <SystemManager.h>
#include <IResource.h>
#include <MathLibrary.h>
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

        const char* materialNames[4]     = {"GlowMatPlanet01", "GlowMatPlanet02", "GlowMatPlanet03", "GlowMatSun"};
        const char* ringMaterialNames[3] = {"Ring01Mat", "Ring02Mat", "Ring03Mat"};
        const char* ringMeshNames[3]     = {"Ring01", "Ring02", "Ring03"};
        const char* planetMeshNames[3]     = {"Planet00", "Planet01", "Planet02"};
        int         m_Stage              = 0;
        EntityHandle CreateGoal(int color, DirectX::XMVECTOR position);

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
        struct TimedFunction
        {
                std::function<void()> m_function;
                float                 m_delay;
        };
        std::vector<TimedFunction> m_timedFunctions;

        static constexpr float goalDistances[4] = {10.0f, 10.0f, 10.0f, 10.0f};

        std::vector<ComponentHandle> sunAlignedTransforms;


        void UpdateSunAlignedObjects(float delta);

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
        uint8_t m_PendingGoalCounts[4] = {0, 0, 0, 0};

        EntityHandle CreateSun();
        EntityHandle CreateRing(int color);
        void InstantCreateOrbitSystem();
        void InstantRemoveOrbitSystem();

        void InstantInOrbit(int color);
        void InstantRemoveFromOrbit();

        unsigned int goalsCollected   = 0;
        bool         collectedMask[3] = {};

        inline void ClearCollectedMask()
        {
                collectedMask[0] = false;
                collectedMask[1] = false;
                collectedMask[2] = false;
        }

		inline void ClearTimedFunctions()
        {
                m_timedFunctions.clear();
		}

        FActiveGoalInfo activeGoal;
        void            DestroyPlanet(GoalComponent* toDestroy);
};