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

void SystemManager::RegisterSystem(FSystemProperties* systemProperties, ISystem* isystem)
{
        assert(systemProperties != nullptr && system != nullptr);

        isystem->SetSystemProperties(*systemProperties);
        isystem->OnInitialize();

        if (systemProperties->m_Flags & SYSTEM_INIT_FLAG_SUSPEND_ON_START) {}
        else
        {
                m_DefaultSystemsQueue.push(isystem);
        }
}

void SystemManager::FilterSystemQueue(int flags)
{
        if (flags == 0)
        {
                m_CurrentSystemQueue = &m_DefaultSystemsQueue;
                return;
        }
        m_FilteredSystemsQueue = SystemQueue();
        auto queue = m_DefaultSystemsQueue;

        while (!queue.empty())
        {
                ISystem* system = queue.top();
				if ((system->GetSystemProperties().m_Flags & flags) > 0) {
                        m_FilteredSystemsQueue.push(system);
				}
                queue.pop();
        }
        m_CurrentSystemQueue = &m_FilteredSystemsQueue;
}

SystemQueue SystemManager::GetSystemQueue()
{
        return *m_CurrentSystemQueue;
}

void SystemManager::Initialize()
{
        m_CurrentSystemQueue = &m_DefaultSystemsQueue;
}

void SystemManager::Shutdown()
{
        for (auto it : m_SystemsMap)
        {
                it.second->OnShutdown();
                delete it.second;
        }
}


