#pragma once
#include <assert.h>
#include <stdint.h>
#include <queue>
#include <unordered_map>
#include "ECSTypes.h"
#include "ISystem.h"

struct FSystemInitProperties
{
        uint16_t mPriority       = 300;
        float    mUpdateRate     = 0.0f;
        bool     bSuspendOnStart = false;
};


class SystemManager
{
    private:
        struct PriorityComparator
        {
                bool operator()(ISystem* lhs, ISystem* rhs)
                {
                        return lhs->mPriority < rhs->mPriority;
                }
        };

        std::unordered_map<SystemTypeID, ISystem*>                                    mSystemsMap;
        std::priority_queue<ISystem*, std::vector<ISystem*>, SystemManager::PriorityComparator> mActiveSystemsQueue;
        std::vector<ISystem*>                                                         mInactiveSystems;

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

        assert(mSystemsMap.find(typeID) == mSystemsMap.end());

        outSystem = new T();
        auto it   = mSystemsMap.insert(std::make_pair(typeID, outSystem));

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
        auto         it     = mSystemsMap.find(typeID);
        static_assert(it != mSystemsMap.end());

        return it->second;
}
