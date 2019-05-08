#pragma once
#include <stdint.h>
#include "ECSTypes.h"
#include "ISystem.h"
#include <queue>
struct SystemProperties;
class SystemManager
{
    public:
        uint16_t GetPriority();
        void     SetPriority(uint16_t priority);
        float    GetUpdateRateMs();
        void     SetUpdateRateMs(float updateRateMs);
        bool     GetSuspendOnStart();
        void     SetSuspendOnStart(bool state);
        template <typename T>
        EResult InitializeSystem(SystemProperties* systemProperties);
        void    Update(float deltaTime);	
		template <typename T>
        void ShutDownSystem();
		template <typename T>
        void SuspendSystem();
		template <typename T>
        void ResumeSystem();
        std::priority_queue<ISystem> GetSystemQueue();
};