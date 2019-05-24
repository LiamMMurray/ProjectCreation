#pragma once

#include "../../Rendering/Components/CameraComponent.h"
#include "../Controller/IController.h"
#include "../Entities/BaseEntities.h"
#include "../GEngine.h"


class ControllerManager
{
    public:
        // bool m_toggleDebugCamera = false;

        // Player Controller Entity
        enum E_CONTROLLERS
        {
                PLAYER,
                DEBUG,
                COUNT
        };

        IController* m_Controllers[E_CONTROLLERS::COUNT] = {};

        SystemManager* m_SystemManager;
        ComponentManager* m_ComponentManager;
        EntityManager* m_EntityManager;

        E_CONTROLLERS m_CurrentController;

		bool m_toggleDebug = false;

        void DisplayConsoleMenu();

        void Initialize();

        void Update(float delta);

        void Shutdown();
};
