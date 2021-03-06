#pragma once

#include <GlobalMacros.h>

#define COMPONENT_MANAGER GEngine::Get()->GetHandleManager()
#define SYSTEM_MANAGER GEngine::Get()->GetSystemManager()
#define GET_SYSTEM(x) GEngine::Get()->GetSystemManager()->GetSystem<x>()
#define ENTITY_MANAGER GEngine::Get()->GetHandleManager()
#define RESOURCE_MANAGER GEngine::Get()->GetResourceManager()

#include <HandleManager.h>
#include <Profiling.h>
#include <ResourceManager.h>
#include <SystemManager.h>
#include <XTime.h>

#include <DirectXMath.h>
#include <LevelStateManager.h>

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
    public:
        bool            m_DebugMode     = false;
        bool            m_GameIsPaused  = false;
        bool            m_WantsGameExit = false;

        LevelStateManager* m_LevelStateManager;


        float m_PlayerRadius          = 0.0f;
        float m_DesiredPlayerRadius   = 0.0f;
        float m_RadiusTransitionSpeed = 5.0f;

        void              Signal();
        DirectX::XMVECTOR m_OriginOffset     = DirectX::XMVECTORF32{0.0f, 0.0f, 0.0f, 0.0f};
        DirectX::XMVECTOR m_WorldOffsetDelta = DirectX::XMVECTORF32{0.0f, 0.0f, 0.0f, 0.0f};
        inline void       SetPlayerRadius(float r)
        {
                m_PlayerRadius = m_DesiredPlayerRadius = r;
        }

        inline void SetDesiredPlayerRadius(float r)
        {
                m_DesiredPlayerRadius = r;
        }

        inline void SetTransitionSpeed(float s)
        {
                m_RadiusTransitionSpeed = s;
        }

        inline float GetCurrentPlayerRadius() const
        {
                return m_PlayerRadius;
        }

        float           m_TerrainAlpha = 0.0f;
        EntityHandle    m_SunHandle;
        ProfilerContext m_MainThreadProfilingContext;

        void        SetGamePaused(bool val);
        inline bool GetGamePaused()
        {
                return m_GameIsPaused;
        }

        static void Initialize();
        static void Shutdown();

        static GEngine* Get();

        float Update();

        HandleManager*          GetHandleManager();
        SystemManager*          GetSystemManager();
        inline ResourceManager* GetResourceManager()
        {
                return m_ResourceManager;
        }

        inline LevelStateManager* GetLevelStateManager()
        {
                return m_LevelStateManager;
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

        inline void ForceSetInstanceReveal(float val)
        {
                m_TargetInstanceReveal = 0.0f;
                m_InstanceReveal       = 0.0f;
        }

        inline void SetPuzzleState(float val)
        {
                m_TargetPuzzleState  = val;
                m_CurrentPuzzleState = val;
        }

        float       m_InstanceReveal       = 0.0f;
        float       m_TargetInstanceReveal = 0.0f;
        float       m_TargetPuzzleState    = 0.0f;
        float       m_CurrentPuzzleState   = 0.0f;
        static bool ShowFPS;
};