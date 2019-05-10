#include "GEngine.h"

GEngine* GEngine::instance;

void GEngine::Initialize()
{
        static GEngine engine;
        instance = &engine;

        instance->mEntityManager = new EntityManager;
        instance->mComponentManager = new ComponentManager;
        instance->mSystemManager    = new SystemManager;

		instance->mSystemManager->Initialize();
}

void GEngine::Shutdown()
{
        instance->mSystemManager->Shutdown();

        delete instance->mEntityManager;
        delete instance->mComponentManager;
        delete instance->mSystemManager;
}

GEngine* GEngine::Get()
{
        assert(instance != nullptr);
        return instance;
}

void GEngine::Signal()
{
        mXTime.Signal();
}

EntityManager* GEngine::GetEntityManager()
{
        return mEntityManager;
}

ComponentManager* GEngine::GetComponentManager()
{
        return mComponentManager;
}

SystemManager* GEngine::GetSystemManager()
{
        return mSystemManager;
}

float GEngine::GetDeltaTime()
{
        return mXTime.Delta();
}

double GEngine::GetTotalTime()
{
        return mXTime.TotalTime();
}
