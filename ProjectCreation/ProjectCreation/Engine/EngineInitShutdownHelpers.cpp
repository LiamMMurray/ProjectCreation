#include "EngineInitShutdownHelpers.h"

#include "..///Engine/GenericComponents/TransformSystem.h"
#include "../Engine/Animation/AnimationSystem.h"
#include "../Engine/Controller/ControllerSystem.h"
#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/Physics/PhysicsSystem.h"

void EngineHelpers::InitEngineSystemManagers(RenderSystem::native_handle_type handle)
{
        GEngine::Initialize();
        AudioManager::Initialize();

        SystemManager* systemManager = GEngine::Get()->GetSystemManager();
        HandleManager* HandleManager = GEngine::Get()->GetHandleManager();


        // Create Render System
        {
                FSystemProperties sysInitProps;
                sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::VERY_LOW;
                sysInitProps.m_UpdateRate = 0.0f;
                sysInitProps.m_Flags      = SYSTEM_FLAG_UPDATE_WHEN_PAUSED;
                RenderSystem* renderSystem;
                systemManager->CreateSystem<RenderSystem>(&renderSystem);
                renderSystem->SetWindowHandle(handle);
                systemManager->RegisterSystem(&sysInitProps, renderSystem);
                renderSystem->m_SystemName = "RenderSystem";
        }

        // Create Physics System
        {
                FSystemProperties sysInitProps;
                sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::VERY_HIGH;
                sysInitProps.m_UpdateRate = 0.0125f;

                PhysicsSystem* physicsSystem;
                systemManager->CreateSystem<PhysicsSystem>(&physicsSystem);
                systemManager->RegisterSystem(&sysInitProps, physicsSystem);
                physicsSystem->m_SystemName = "PhysicsSystem";
        }

        // Create Animation System
        {
                FSystemProperties sysInitProps;
                sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::NORMAL;
                sysInitProps.m_UpdateRate = 0.0f;

                AnimationSystem* animSystem;
                systemManager->CreateSystem<AnimationSystem>(&animSystem);
                systemManager->RegisterSystem(&sysInitProps, animSystem);
                animSystem->m_SystemName = "AnimSystem";
        }

        // Create Controller System
        {
                FSystemProperties sysInitProps;
                sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::VERY_HIGH;
                sysInitProps.m_UpdateRate = 0.0f;

                ControllerSystem* controllerSystem;
                systemManager->CreateSystem<ControllerSystem>(&controllerSystem);
                systemManager->RegisterSystem(&sysInitProps, controllerSystem);
                controllerSystem->m_SystemName = "ControllerSystem";
        }

        // Create Transform System
        {
                FSystemProperties sysInitProps;
                sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::VERY_HIGH;
                sysInitProps.m_UpdateRate = 0.0f;

                TransformSystem* transformSystem;
                systemManager->CreateSystem<TransformSystem>(&transformSystem);
                systemManager->RegisterSystem(&sysInitProps, transformSystem);
                transformSystem->m_SystemName = "TransformSystem";
        }

        GCoreInput::InitializeInput((HWND)handle);
}

void EngineHelpers::ShutdownEngineSystemManagers()
{
        // ControllerSystem::Shutdown();
        AudioManager::Shutdown();
        GEngine::Shutdown();
        delete GEngine::Get();
}
