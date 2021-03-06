#pragma once
#include <queue>
#include <unordered_map>
#include <vector>
#define KB(x) ((size_t)(x) << 10)
#define MB(x) ((size_t)(x) << 20)

struct ComponentHandle;

namespace NMemory
{
        // todo forward declare typedefs (requires forward declaring std::containers)
        typedef char                              byte;
        typedef size_t                            memsize;
        typedef uint32_t                          index;
        typedef uint32_t                          type_index;
        typedef std::vector<type_index>           type_indices;
        typedef std::vector<index>                indices;
        typedef std::vector<bool>                 dynamic_bitset; // STL specialize vector<bool> as a dynamic bitset
        typedef size_t                            deletion_accumulator;
        typedef std::vector<deletion_accumulator> deletion_accumulators;
        typedef std::vector<index>                delete_requests;
        typedef std::priority_queue<index, type_indices, std::greater<index>> index_priority_queue;
        typedef size_t                                                        entity_index;

        struct MemoryStack
        {
                byte* m_MemStart = 0;
                byte* m_MemCurr  = 0;
                byte* m_MemMax   = 0;
        };


        // Allocates memory on the system level
        void ReserveGameMemory(MemoryStack& poolMemory, memsize allocSize);
        void FreeGameMemory(MemoryStack& poolMemory);
} // namespace NMemory
