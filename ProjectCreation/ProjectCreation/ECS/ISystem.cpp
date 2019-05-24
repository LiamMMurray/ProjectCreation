#include "ISystem.h"

SystemTypeId ISystem::systemTypeId = 0;

uint16_t ISystem::GetPriority()
{
        return m_Priority;
}

void ISystem::SetPriority(uint16_t priority)
{
        m_Priority = priority;
}

float ISystem::GetUpdateRateMs()
{
        return m_UpdateRateMs;
}

void ISystem::SetUpdateRateMs(float updateRateMs)
{
        m_UpdateRateMs = updateRateMs;
}