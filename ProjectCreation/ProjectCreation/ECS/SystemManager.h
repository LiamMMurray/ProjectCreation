#pragma once
#include <assert.h>
#include <stdint.h>
#include <queue>
#include <unordered_map>
#include "ECSTypes.h"
#include "ISystem.h"

struct FSystemInitProperties
{
        uint16_t m_Priority       = 300;
        float    m_UpdateRate     = 0.0f;
        bool     bSuspendOnStart = false;
};


class SystemManager
{
    private:
        struct PriorityComparator
        {
                bool operator()(ISystem* lhs, ISystem* rhs)
                {
                        return lhs->m_Priority < rhs->m_Priority;
                }
        };

        std::unordered_map<SystemTypeID, ISystem*>                                    m_SystemsMap;
        std::priority_queue<ISystem*, std::vector<ISystem*>, SystemManager::PriorityComparator> m_ActiveSystemsQueue;
        std::vector<ISystem*>                                                         m_InactiveSystems;

    public:
        template <typename T>
        EResult CreateSystem(T* outSystem);
        void    Update(float deltaTime);
        template <typename T>
        void ShutdownSystem();
        template <typename T>
        void InitializeSystem(FSystemInitProperties* systemProperties);

        void InitializeSystem(FSystemInitProperties* systemProperties, ISystem* isystem);
        template <typename T>
        void SuspendSystem();
        template <typename T>
        void ResumeSystem();
        template <typename T>
        T* GetSystem();

        std::priority_queue<ISystem*, std::vector<ISystem*>, SystemManager::PriorityComparator> GetSystemQueue();

		void Initialize();
        void Shutdown();
};

template <typename T>
inline EResult SystemManager::CreateSystem(T* outSystem)
{
        static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");
        assert(outSystem != nullptr);

        SystemTypeID typeID = ISystem::GetTypeID<T>();

        assert(m_SystemsMap.find(typeID) == m_SystemsMap.end());

        outSystem = new T();
        auto it   = m_SystemsMap.insert(std::make_pair(typeID, outSystem));

        EResult output;
        output.m_Flags = ERESULT_FLAG::SUCCESS;

        return output;
}

template <typename T>
inline void SystemManager::InitializeSystem(FSystemInitProperties* systemProperties)
{
        ISystem* system = GetSystem<T>();
        InitializeSystem(systemProperties, system);
}

template <typename T>
inline void SystemManager::ShutdownSystem()
{
        static_assert(std::is_base_of<ISystem, T>::value, "Error. Template type must be subclass of ISystem");
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
        SystemTypeID typeID = ISystem::GetTypeID<T>();
        auto         it     = m_SystemsMap.find(typeID);
        static_assert(it != m_SystemsMap.end());

        return it->second;
}
