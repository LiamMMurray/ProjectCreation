#include "GEngine.h"



GEngine* GEngine::instance;

void GEngine::Initialize()
{
        static GEngine engine;
        instance = &engine;

        instance->m_EntityManager    = new EntityManager;
        instance->m_ComponentManager = new ComponentManager;
        instance->m_SystemManager    = new SystemManager;
        instance->m_ResourceManager  = new ResourceManager;

		instance->m_EntityManager->Initialize(instance->m_ComponentManager);
        instance->m_SystemManager->Initialize();
        instance->m_ResourceManager->Initialize();
}

void GEngine::Shutdown()
{
        instance->m_SystemManager->Shutdown();
        instance->m_ResourceManager->Shutdown();

        delete instance->m_EntityManager;
        delete instance->m_ComponentManager;
        delete instance->m_SystemManager;
        delete instance->m_ResourceManager;
}

GEngine* GEngine::Get()
{
        assert(instance != nullptr);
        return instance;
}

void GEngine::Signal()
{
        m_XTime.Signal();
}

EntityManager* GEngine::GetEntityManager()
{
        return m_EntityManager;
}

ComponentManager* GEngine::GetComponentManager()
{
        return m_ComponentManager;
}

SystemManager* GEngine::GetSystemManager()
{
        return m_SystemManager;
}

float GEngine::GetDeltaTime()
{
        return (float)m_XTime.Delta();
}

double GEngine::GetTotalTime()
{
        return m_XTime.TotalTime();
}
