#include "EngineInitShutdownHelpers.h"

#include "../Engine/CoreInput/CoreInput.h"
#include "../Engine/Animation/AnimationSystem.h"
#include "../Engine/Physics/PhysicsSystem.h"

void EngineHelpers::InitEngineSystemManagers(RenderSystem::native_handle_type handle)
{
        GEngine::Initialize();

        SystemManager*    systemManager    = GEngine::Get()->GetSystemManager();
        EntityManager*    entityManager    = GEngine::Get()->GetEntityManager();
        ComponentManager* componentManager = GEngine::Get()->GetComponentManager();

        // Create Render System
        RenderSystem* renderSystem;
        systemManager->CreateSystem<RenderSystem>(&renderSystem);
        FSystemInitProperties sysInitProps;
        renderSystem->SetWindowHandle(handle);
        systemManager->RegisterSystem(&sysInitProps, renderSystem);

        // Create Physics System
        PhysicsSystem* physicsSystem;
        systemManager->CreateSystem<PhysicsSystem>(&physicsSystem);
        sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::VERY_HIGH;
        sysInitProps.m_UpdateRate = 0.0125f;
        systemManager->RegisterSystem(&sysInitProps, physicsSystem);

        // Create Animation System
        AnimationSystem* animSystem;
        systemManager->CreateSystem<AnimationSystem>(&animSystem);
        sysInitProps.m_Priority   = E_SYSTEM_PRIORITY::NORMAL;
        sysInitProps.m_UpdateRate = 0.0f;
        systemManager->RegisterSystem(&sysInitProps, animSystem);

        GCoreInput::InitializeInput((HWND)handle);

        AudioManager::Initialize();
}

void EngineHelpers::ShutdownEngineSystemManagers()
{
        AudioManager::Shutdown();
        GEngine::Shutdown();
}
