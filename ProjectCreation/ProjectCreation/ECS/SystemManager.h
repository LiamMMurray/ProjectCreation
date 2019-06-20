#pragma once
#include <assert.h>
#include <stdint.h>
#include <queue>
#include <unordered_map>
#include "../ErrorHandling/ErrorTypes.h"
#include "../Utility/MemoryLeakDetection.h"
#include "ECSTypes.h"
#include "ISystem.h"

struct PriorityComparator
{
        bool operator()(ISystem* lhs, ISystem* rhs)
        {
                return lhs->GetSystemProperties().m_Priority < rhs->GetSystemProperties().m_Priority;
        }
};

using SystemQueue = std::priority_queue<ISystem*, std::vector<ISystem*>, PriorityComparator>;

class SystemManager
{
    public:
        template <typename T>
        EResult CreateSystem(T** outSystem);
        void    Update(float deltaTime);
        template <typename T>
        void DestroySystem();
        template <typename T>
        void RegisterSystem(FSystemProperties* systemProperties);

        void RegisterSystem(FSystemProperties* systemProperties, ISystem* isystem);
        template <typename T>
        void SuspendSystem();
        template <typename T>
        void ResumeSystem();
        template <typename T>
        T* GetSystem();

        void FilterSystemQueue(int flags = 0);
        SystemQueue GetSystemQueue();

        void Initialize();
        void Shutdown();

    private:
        std::unordered_map<SystemTypeId, ISystem*> m_SystemsMap;
        SystemQueue                                m_DefaultSystemsQueue;
        SystemQueue                                m_FilteredSystemsQueue;
        SystemQueue*                               m_CurrentSystemQueue;
};

template <typename T>
inline EResult SystemManager::CreateSystem(T** outSystem)
{
        static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");
        assert(outSystem != nullptr);

        SystemTypeId typeID = ISystem::GetTypeID<T>();

        assert(m_SystemsMap.find(typeID) == m_SystemsMap.end());

        *outSystem               = DBG_NEW T();
        ISystem* val = *outSystem;
        auto     it  = m_SystemsMap.insert(std::make_pair(typeID, val));

        EResult output;
        output.m_Flags = ERESULT_FLAG::SUCCESS;

        return output;
}

template <typename T>
inline void SystemManager::RegisterSystem(FSystemProperties* systemProperties)
{
        ISystem* system = GetSystem<T>();
        RegisterSystem(systemProperties, system);
}

template <typename T>
inline void SystemManager::DestroySystem()
{
        static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");

        ISystem*     system = GetSystem<T>();
        SystemTypeID typeID = ISystem::GetTypeID<T>();
        assert(m_SystemsMap.find(typeID) == m_SystemsMap.end());

        system->OnShutdown();
        m_SystemsMap.erase(typeID);
}

template <typename T>
inline void SystemManager::SuspendSystem()
{
        static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");
}

template <typename T>
inline void SystemManager::ResumeSystem()
{
        static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");
}

template <typename T>
T* SystemManager::GetSystem()
{
        static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");
        SystemTypeId typeID = ISystem::GetTypeID<T>();
        auto         it     = m_SystemsMap.find(typeID);
        assert(it != m_SystemsMap.end());

        return (T*)it->second;
}
