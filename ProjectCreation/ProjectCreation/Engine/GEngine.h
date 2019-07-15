#pragma once

#define COMPONENT_MANAGER GEngine::Get()->GetHandleManager()
#define SYSTEM_MANAGER GEngine::Get()->GetSystemManager()
#define ENTITY_MANAGER GEngine::Get()->GetHandleManager()
#define RESOURCE_MANAGER GEngine::Get()->GetResourceManager()

#include "../ECS/HandleManager.h"
#include "../ECS/SystemManager.h"
#include "ResourceManager/ResourceManager.h"
#include "XTime.h"
class GEngine
{

        static NMemory::memsize            s_PoolAllocSize;
        NMemory::MemoryStack               m_PoolMemory;
        NMemory::NPools::RandomAccessPools m_ComponentPools;
        NMemory::NPools::RandomAccessPools m_EntityPools;

        HandleManager*   m_HandleManager;
        SystemManager*   m_SystemManager;
        ResourceManager* m_ResourceManager;

        XTime m_XTime;


        static GEngine* instance;

        bool m_DebugMode     = false;
        bool m_GameIsPaused  = false;
        bool m_WantsGameExit = false;

    public:
        float        m_PlayerRadius = 0.0f;
        float        m_TerrainAlpha = 0.0f;
        EntityHandle m_SunHandle;

        void        SetGamePaused(bool val);
        inline bool GetGamePaused()
        {
                return m_GameIsPaused;
        }

        static void Initialize();
        static void Shutdown();

        static GEngine* Get();

        void Signal();

        HandleManager*          GetHandleManager();
        SystemManager*          GetSystemManager();
        inline ResourceManager* GetResourceManager()
        {
                return m_ResourceManager;
        }

        inline void SetDebugMode(bool val)
        {
                m_DebugMode = val;
        }
        inline bool IsDebugMode() const
        {
                return m_DebugMode;
        }
        inline void ToggleDebugMode()
        {
                m_DebugMode = !m_DebugMode;
        }

        float  GetDeltaTime();
        double GetTotalTime();

        inline bool WantsGameExit()
        {
                return m_WantsGameExit;
        }

        inline void RequestGameExit()
        {
                m_WantsGameExit = true;
        }
};