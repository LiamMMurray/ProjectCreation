#pragma once

#include "../ECS/EntityManager.h"
#include "../ECS/SystemManager.h"
#include "XTime.h"
class GEngine
{
        EntityManager*    mEntityManager;
        ComponentManager* mComponentManager;
        SystemManager*    mSystemManager;
        XTime             mXTime;			  		  

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