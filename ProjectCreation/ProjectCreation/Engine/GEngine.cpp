#include "GEngine.h"

GEngine*                           GEngine::instance       = 0;
NMemory::memsize                   GEngine::gameMemorySize = 80000000;
NMemory::NPools::RandomAccessPools GEngine::m_component_pools;
NMemory::NPools::RandomAccessPools GEngine::m_entity_pools;

void GEngine::SetGamePaused(bool val)
{
        m_GameIsPaused = val;
        if (m_GameIsPaused)
        {
                m_SystemManager->FilterSystemQueue(SYSTEM_FLAG_UPDATE_WHEN_PAUSED);
        }
        else
        {
                m_SystemManager->FilterSystemQueue();
        }
}

void GEngine::Initialize()
{
        instance = new GEngine;

        NMemory::GameMemory_Singleton::GameMemory_Start = NMemory::ReserveGameMemory(gameMemorySize);
        assert(NMemory::GameMemory_Singleton::GameMemory_Start != 0);
        NMemory::GameMemory_Singleton::GameMemory_Curr = NMemory::GameMemory_Singleton::GameMemory_Start;
        NMemory::GameMemory_Singleton::GameMemory_Max  = NMemory::GameMemory_Singleton::GameMemory_Start + gameMemorySize;


        instance->m_HandleManager =
            new HandleManager(m_component_pools, m_entity_pools, NMemory::GameMemory_Singleton::GameMemory_Curr);

        
        instance->m_SystemManager   = new SystemManager;
        instance->m_ResourceManager = new ResourceManager;

        instance->m_SystemManager->Initialize();
        instance->m_ResourceManager->Initialize();
}

void GEngine::Shutdown()
{
        instance->m_SystemManager->Shutdown();
        instance->m_ResourceManager->Shutdown();

        delete instance->m_HandleManager;
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

HandleManager* GEngine::GetHandleManager()
{
        return m_HandleManager;
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
