#include "SystemManager.h"

void SystemManager::Update(float deltaTime)
{
        auto queue = GetSystemQueue();

        while (!queue.empty())
        {
                ISystem* system = queue.top();
                system->OnPreUpdate(deltaTime);
                system->OnUpdate(deltaTime);
                system->OnPostUpdate(deltaTime);
                queue.pop();
        }
}

void SystemManager::RegisterSystem(FSystemInitProperties* systemProperties, ISystem* isystem)
{
        assert(systemProperties != nullptr && system != nullptr);

        isystem->OnInitialize();

        if (systemProperties->m_Flags & SYSTEM_INIT_FLAG_SUSPEND_ON_START)
        {
                m_InactiveSystems.push_back(isystem);
        }
        else
        {
                m_ActiveSystemsQueue.push(isystem);
        }
}

std::priority_queue<ISystem*, std::vector<ISystem*>, SystemManager::PriorityComparator> SystemManager::GetSystemQueue()
{
        return m_ActiveSystemsQueue;
}

void SystemManager::Initialize()
{}

void SystemManager::Shutdown()
{
        for (auto it : m_SystemsMap)
        {
                it.second->OnShutdown();
                delete it.second;
        }
}
