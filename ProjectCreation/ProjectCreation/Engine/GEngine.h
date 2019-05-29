#pragma once

#include "../ECS/ECS.h"
#include "ResourceManager/ResourceManager.h"
#include "XTime.h"
class GEngine
{
        EntityManager*    m_EntityManager;
        ComponentManager* m_ComponentManager;
        SystemManager*    m_SystemManager;
        ResourceManager*  m_ResourceManager;

        XTime m_XTime;


        static GEngine* instance;

        bool m_DebugMode = false;

    public:
        float m_PlayerRadius = 0.0f;

        static void Initialize();
        static void Shutdown();

        static GEngine* Get();

        void Signal();

        EntityManager*          GetEntityManager();
        ComponentManager*       GetComponentManager();
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
};