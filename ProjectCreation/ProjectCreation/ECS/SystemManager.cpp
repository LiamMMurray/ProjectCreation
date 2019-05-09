#include "SystemManager.h"

uint16_t SystemManager::GetPriority()
{
        return uint16_t();
}

void SystemManager::SetPriority(uint16_t priority)
{}

float SystemManager::GetUpdateRateMs()
{
        return 0.0f;
}

void SystemManager::SetUpdateRateMs(float updateRateMs)
{}

bool SystemManager::GetSuspendOnStart()
{
        return false;
}

void SystemManager::SetSuspendOnStart(bool state)
{}

void SystemManager::Update(float deltaTime)
{}

std::priority_queue<ISystem> SystemManager::GetSystemQueue()
{
        return std::priority_queue<ISystem>();
}
