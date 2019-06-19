#include "Memory.h"
#define WIN_32_LEAN_AND_MEAN
#include <Windows.h>
#include <assert.h>
#include <malloc.h>
#include "../Utility/MemoryLeakDetection.h"

namespace NMemory
{
        void ReserveGameMemory(PoolMemory& poolMemory, memsize allocSize)
        {
                // LPVOID ptr = VirtualAlloc(0, allocSize, MEM_RESERVE, PAGE_READWRITE);
                // ptr        = VirtualAlloc(ptr, allocSize, MEM_COMMIT, PAGE_READWRITE);
                // return static_cast<byte*>(ptr);
                poolMemory.m_MemStart = reinterpret_cast<byte*>(malloc(allocSize));
                poolMemory.m_MemCurr  = poolMemory.m_MemStart;
                poolMemory.m_MemMax   = poolMemory.m_MemStart + allocSize;
        }
        void FreeGameMemory(PoolMemory& poolMemory)
        {
                // int error = VirtualFree(GameMemory_Singleton::GameMemory_Start, 0, MEM_RELEASE);

                // if (error == 0)
                //{
                //        auto error = GetLastError();
                //        assert(false);
                //}
                // free(GameMemory_Singleton::GameMemory_Start);
                free(poolMemory.m_MemStart);
        }
}; // namespace NMemory