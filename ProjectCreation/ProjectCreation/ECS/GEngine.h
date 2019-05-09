#pragma once
#include "ComponentManager.h"
#include "EntityManager.h"
#include "SystemManager.h"
class GEngine
{
    public:
        EntityManager*    GetEntityManager();
        ComponentManager* GetComponentManager();
        SystemManager*    GetSystemManager();
        float             GetDeltaTime();
        double            GetTotalTime();
};