#include "GEngine.h"
#include "../Utility/MemoryLeakDetection.h"
#include "MathLibrary/MathLibrary.h"

GEngine*         GEngine::instance        = 0;
NMemory::memsize GEngine::s_PoolAllocSize = MB(64);

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
        instance = DBG_NEW GEngine;

        NMemory::ReserveGameMemory(instance->m_PoolMemory, s_PoolAllocSize);


        instance->m_HandleManager =
            DBG_NEW HandleManager(instance->m_ComponentPools, instance->m_EntityPools, instance->m_PoolMemory);

        instance->m_MainThreadProfilingContext.Initialize();

        instance->m_SystemManager     = DBG_NEW     SystemManager;
        instance->m_ResourceManager   = DBG_NEW   ResourceManager;
        instance->m_LevelStateManager = DBG_NEW LevelStateManager;

        instance->m_SystemManager->Initialize();
        instance->m_ResourceManager->Initialize();
}

void GEngine::Shutdown()
{
        instance->m_SystemManager->Shutdown();
        instance->m_ResourceManager->Shutdown();
        instance->m_HandleManager->Shutdown();
        NMemory::FreeGameMemory(instance->m_PoolMemory);
        delete instance->m_HandleManager;
        delete instance->m_SystemManager;
        delete instance->m_ResourceManager;
        delete instance->m_LevelStateManager;

        GEngine::Get()->m_MainThreadProfilingContext.Dump();
}

GEngine* GEngine::Get()
{
        assert(instance != nullptr);
        return instance;
}

float GEngine::Update()
{
        Signal();
        float deltaTime = GetDeltaTime();

        m_PlayerRadius = MathLibrary::MoveTowards(m_PlayerRadius, m_DesiredPlayerRadius, m_RadiusTransitionSpeed * deltaTime);
        GetLevelStateManager()->Update(deltaTime);
        return deltaTime;
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
