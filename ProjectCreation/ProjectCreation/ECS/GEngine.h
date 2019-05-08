#pragma once
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"
class GEngine
{
    public:
        EntityManager* GetEntityManager();
        ComponentManager* GetComponentManager();
        SystemManager*    GetSystemManager();
        float             GetDeltaTime();
        double            GetTotalTime();
};