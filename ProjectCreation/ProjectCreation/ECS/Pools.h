#pragma once
#include "Memory.h"

namespace NMemory
{
        namespace NPools
        {
                struct Pool_Desc
                {
                        memsize element_size;
                        index   element_capacity;
                };
                typedef std::vector<Pool_Desc> pool_descs;


                struct ForwardAccessPools
                {
                        static const memsize        s_pool_alignment_boundary;
                        std::vector<byte*>          m_mem_starts;
                        std::vector<index>          m_element_counts;
                        std::vector<memsize>        m_elment_byte_sizes;
                        std::vector<index>          m_element_capacities;
                        std::vector<dynamic_bitset> m_element_isactives;
                };

                struct Allocation
                {
                        index redirection_idx;
                        byte* objectPtr;
                };

                void AppendPools(ForwardAccessPools& pools, const pool_descs& _pool_descs, byte*& dynamic_mem);

                struct RandomAccessPools : public ForwardAccessPools
                {
                        std::vector<indices> m_redirection_indices; // redirection_and_isactive_indices would be more accurate
                        std::vector<index_priority_queue> m_free_redirection_indices;
                };

                void AppendPools(RandomAccessPools& forward_access_pools, const pool_descs& _pool_descs, byte*& dynamic_mem);

                void InsertPool(RandomAccessPools& pools,
                                const Pool_Desc&   _pool_desc,
                                byte*&             dynamic_mem,
                                type_index         pool_index);

                void ClearPools(RandomAccessPools& pools, type_indices pool_indices);

                void ClearPools(RandomAccessPools& pools);

                byte* GetData(RandomAccessPools& component_random_access_pools, index pool_index, index data_index);

                void Free(RandomAccessPools& component_random_access_pools, index pool_index, indices& index_buffer_indexes);

                Allocation Allocate(RandomAccessPools& component_random_access_pools, index pool_index);

                void ReleaseRedirectionIndices(RandomAccessPools& component_random_access_pools,
                                               index              pool_index,
                                               indices&           redirection_indices);
        } // namespace NPools
} // namespace NMemory