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

// this file must be the first included file in the project to ensure the _StaticSentinelDumpMemoryLeaks object is destructed last in static memory
// https://en.cppreference.com/w/cpp/utility/program/exit static objects are destructed in the reverse order they are constructed

inline static StaticSentinelDumpMemoryLeaks _StaticSentinelDumpMemoryLeaks;