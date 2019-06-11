#pragma once

#include "../../Rendering/Components/CameraComponent.h"
#include "../Controller/IController.h"
#include "../Entities/BaseEntities.h"
#include "..//..//ECS/ECS.h"
#include "../GEngine.h"
#include "../Gameplay/LightOrbColors.h"

class ControllerSystem : public ISystem
{
        static ControllerSystem* instance;

        void init();
        void shutdown();

        void update(float delta);

    public:
        // bool m_toggleDebugCamera = false;

        // Player Controller Entity
        enum E_CONTROLLERS
        {
                PLAYER,
                DEBUG,
                COUNT
        };

        inline IController* GetCurrentController()
        {
                return m_Controllers[m_CurrentController];
        }

        IController* m_Controllers[E_CONTROLLERS::COUNT] = {};

        SystemManager*    m_SystemManager;
        ComponentManager* m_ComponentManager;
        EntityManager*    m_EntityManager;

        E_CONTROLLERS m_CurrentController;

        int m_RedOrbCount   = 0;
        int m_BlueOrbCount  = 0;
        int m_GreenOrbCount = 0;

        void DisplayConsoleMenu();

        // Returns the number of orbs collected of a certain color based on passed in parameter
        // 0 = Red Lights, 1 = Blue Lights, 2 = Green Lights
        int GetOrbCount(E_LIGHT_ORBS color);

        // Adds one orb to a certain color based on passed in parameter
        // 0 = Red Lights, 1 = Blue Lights, 2 = Green Lights
        void SetOrbCount(E_LIGHT_ORBS color);

        // Inherited via ISystem
        virtual void OnPreUpdate(float deltaTime) override;
        virtual void OnUpdate(float deltaTime) override;
        virtual void OnPostUpdate(float deltaTime) override;
        virtual void OnInitialize() override;
        virtual void OnShutdown() override;
        virtual void OnResume() override;
        virtual void OnSuspend() override;
};
