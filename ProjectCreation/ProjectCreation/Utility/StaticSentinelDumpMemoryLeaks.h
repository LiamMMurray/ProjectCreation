#pragma once
#include "MemoryLeakDetection.h"
struct StaticSentinelDumpMemoryLeaks
{
        StaticSentinelDumpMemoryLeaks()
        {}
        ~StaticSentinelDumpMemoryLeaks()
        {
                _CrtDumpMemoryLeaks();
        }
};
static StaticSentinelDumpMemoryLeaks _StaticSentinelDumpMemoryLeaks;