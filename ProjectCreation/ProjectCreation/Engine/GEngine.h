#pragma once

#include "../ECS/ECS.h"
#include "XTime.h"
class GEngine
{
        EntityManager*    m_EntityManager;
        ComponentManager* m_ComponentManager;
        SystemManager*    m_SystemManager;
        XTime             m_XTime;


		static GEngine* instance;

    public:
        static void Initialize();
        static void  Shutdown();

        static GEngine* Get();

		void Signal();

        EntityManager*    GetEntityManager();
        ComponentManager* GetComponentManager();
        SystemManager*    GetSystemManager();
        float             GetDeltaTime();
        double            GetTotalTime();
};