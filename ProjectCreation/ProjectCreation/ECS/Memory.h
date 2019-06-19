#pragma once
#include <queue>
#include <unordered_map>
#include <vector>
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
        typedef std::unordered_multimap<type_index, index>                    entity_component_container;
        typedef std::pair<entity_component_container::iterator, entity_component_container::iterator> entity_components;

        struct GameMemory_Singleton
        {
            public:
                static byte* GameMemory_Start;
                static byte* GameMemory_Curr;
                static byte* GameMemory_Max;
        };


        // Allocates memory on the system level
        byte* ReserveGameMemory(memsize memsize);
        void  FreeGameMemory();
} // namespace NMemory
