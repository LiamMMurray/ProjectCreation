#include "ISystem.h"

SystemTypeID ISystem::systemTypeID = 0;

uint16_t ISystem::GetPriority()
{
        return mPriority;
}

void ISystem::SetPriority(uint16_t priority)
{
        mPriority = priority;
}

float ISystem::GetUpdateRateMs()
{
        return mUpdateRateMs;
}

void ISystem::SetUpdateRateMs(float updateRateMs)
{
        mUpdateRateMs = updateRateMs;
}