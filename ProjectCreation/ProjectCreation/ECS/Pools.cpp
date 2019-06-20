#include "Pools.h"
#include <assert.h>
#include <algorithm>
#include "HandleManager.h"
#include "IComponent.h"
namespace NMemory
{
        namespace NPools
        {
                const memsize ForwardAccessPools::s_pool_alignment_boundary = 64;
                // intializes pool data
                // mem_start gets modified to be new mem start
                void AppendPools(ForwardAccessPools& pools, const pool_descs& _pool_descs, byte*& dynamic_mem)
                {
                        index pool_count      = pools.m_element_capacities.size();
                        index pool_desc_count = _pool_descs.size();
                        index new_pool_count  = pool_count + pool_desc_count;

                        // reserve memory in vectors
                        for (index i = pool_count; i < new_pool_count; i++)
                        {
                                pools.m_element_capacities.resize(new_pool_count);
                                pools.m_elment_byte_sizes.resize(new_pool_count);
                                pools.m_mem_starts.resize(new_pool_count);
                        }
                        // iterate and set data
                        for (index i = pool_count; i < new_pool_count; i++)
                        {
                                pools.m_element_capacities[i] = _pool_descs[i].element_capacity;
                                pools.m_elment_byte_sizes[i]  = _pool_descs[i].element_size;
                                pools.m_mem_starts[i]         = dynamic_mem;
                                dynamic_mem += _pool_descs[i].element_capacity * _pool_descs[i].element_size;
                        }
                }

                // intializes pool data
                // mem_start gets modified to be new mem start
                void AppendPools(RandomAccessPools& pools, const pool_descs& _pool_descs, byte*& dynamic_mem)
                {
                        type_index pool_count      = static_cast<type_index>(pools.m_element_capacities.size());
                        type_index pool_desc_count = _pool_descs.size();
                        type_index new_pool_count  = pool_count + pool_desc_count;

                        /// resize all element vectors
                        for (type_index i = pool_count; i < new_pool_count; i++)
                        {
                                pools.m_element_capacities.resize(new_pool_count);
                                pools.m_redirection_indices.resize(new_pool_count);
                                pools.m_free_redirection_indices.resize(new_pool_count);
                                pools.m_elment_byte_sizes.resize(new_pool_count);
                                pools.m_element_counts.resize(new_pool_count);
                                pools.m_mem_starts.resize(new_pool_count);
                                pools.m_element_isactives.resize(new_pool_count);
                        }

                        type_index _i_descs = 0;
                        type_index _i_pool  = pool_count;
                        for (; _i_pool < new_pool_count; _i_pool++, _i_descs++)
                        {
                                pools.m_element_capacities[_i_pool] = _pool_descs[_i_descs].element_capacity;
                                pools.m_elment_byte_sizes[_i_pool]  = _pool_descs[_i_descs].element_size;
                                pools.m_mem_starts[_i_pool]         = dynamic_mem;
                                dynamic_mem += _pool_descs[_i_descs].element_capacity * _pool_descs[_i_descs].element_size;

                                index index_count = _pool_descs[_i_descs].element_capacity;
                                pools.m_redirection_indices[_i_pool].resize(static_cast<size_t>(index_count));
                                pools.m_element_isactives[_i_pool].resize(static_cast<size_t>(index_count));
                                for (index j = 0; j < index_count; j++)
                                {
                                        pools.m_free_redirection_indices[_i_pool].push(j);
                                }
                        }
                }

                void InsertPool(RandomAccessPools& pools,
                                const Pool_Desc&   _pool_desc,
                                byte*&             dynamic_mem,
                                type_index         pool_index)
                {
                        type_index pool_count = pools.m_element_capacities.size();

                        if (pool_count <= pool_index)
                        {
                                pool_descs _pool_descs;
                                _pool_descs.resize(pool_index - (pool_count - 1));
                                for (auto& _ps : _pool_descs)
                                {
                                        _ps = {};
                                }
                                _pool_descs[pool_index - (pool_count - 1) - 1].element_capacity = _pool_desc.element_capacity;
                                _pool_descs[pool_index - (pool_count - 1) - 1].element_size     = _pool_desc.element_size;
                                AppendPools(pools, _pool_descs, dynamic_mem);
                        }
                        else
                        {
                                pools.m_element_capacities[pool_index] = _pool_desc.element_capacity;
                                pools.m_elment_byte_sizes[pool_index]  = _pool_desc.element_size;
                                pools.m_mem_starts[pool_index]         = dynamic_mem;
                                dynamic_mem += _pool_desc.element_capacity * _pool_desc.element_size;
                                while ((uint64_t)dynamic_mem % (uint64_t)RandomAccessPools::s_pool_alignment_boundary != 0)
                                {
                                        dynamic_mem += 1;
                                }
                                index index_count = _pool_desc.element_capacity;
                                pools.m_redirection_indices[pool_index].resize(index_count);
                                pools.m_element_isactives[pool_index].resize(index_count);
                                for (index j = 0; j < index_count; j++)
                                {
                                        pools.m_free_redirection_indices[pool_index].push(j);
                                }
                        }
                }

                void ClearPools(RandomAccessPools& pools, type_indices pool_indices)
                {
                        for (type_index i = 0; i < pool_indices.size(); i++)
                        {
                                type_index pool_index        = pool_indices[i];
                                memsize    element_size      = pools.m_elment_byte_sizes[pool_index];
                                byte*      element_mem_start = pools.m_mem_starts[pool_index];
                                index      element_index_end = pools.m_element_counts[pool_index];

                                for (index element_index = 0; element_index < element_index_end; element_index++)
                                {
                                        IPoolElement* pool_element_interface =
                                            reinterpret_cast<IPoolElement*>(element_mem_start + element_index * element_size);
                                        pool_element_interface->~IPoolElement();
                                }
                                pools.m_element_counts[pool_index] = 0;
                        }
                }

                void ClearPools(RandomAccessPools& pools)
                {
                        type_index pool_count = pools.m_mem_starts.size();
                        for (type_index i = 0; i < pool_count; i++)
                        {
                                type_index pool_index        = i;
                                memsize    element_size      = pools.m_elment_byte_sizes[pool_index];
                                byte*      element_mem_start = pools.m_mem_starts[pool_index];
                                index      element_index_end = pools.m_element_counts[pool_index];

                                for (index element_index = 0; element_index < element_index_end; element_index++)
                                {
                                        IPoolElement* pool_element_interface =
                                            reinterpret_cast<IPoolElement*>(element_mem_start + element_index * element_size);
                                        pool_element_interface->~IPoolElement();
                                }
                                pools.m_element_counts[pool_index] = 0;
                        }
                }

                byte* GetData(RandomAccessPools& component_random_access_pools, index pool_index, index index_buffer_index)
                {
                        index element_index =
                            component_random_access_pools.m_redirection_indices[pool_index][index_buffer_index];
                        if (element_index == -1)
                                return 0;

                        memsize element_size = component_random_access_pools.m_elment_byte_sizes[pool_index];
                        byte*   mem_start    = component_random_access_pools.m_mem_starts[pool_index];

                        return mem_start + element_index * element_size;
                }

                void Free(RandomAccessPools& component_random_access_pools,
                          index              pool_index,
                          indices&           deleted_redirection_indices)
                {
                        memsize element_size       = component_random_access_pools.m_elment_byte_sizes[pool_index];
                        byte*   mem_start          = component_random_access_pools.m_mem_starts[pool_index];
                        index   last_element_index = component_random_access_pools.m_element_counts[pool_index] - 1;

                        for (index i = 0; i < deleted_redirection_indices.size(); i++)
                        {
                                // this  function performs a sorted pool's function copying over the memory of the deleted
                                // element with that of the last element.
                                // It also includes a priority queue of indices to maintain persistent identity and random order
                                // access.
                                index deleted_redirection_index = deleted_redirection_indices[i];

                                index deleted_element_index =
                                    component_random_access_pools.m_redirection_indices[pool_index][deleted_redirection_index];

                                byte* last_element_mem    = mem_start + last_element_index * element_size;
                                byte* deleted_element_mem = mem_start + deleted_element_index * element_size;

                                // call the element's virtual destructor
                                IPoolElement* pool_element_interface = reinterpret_cast<IPoolElement*>(deleted_element_mem);
                                pool_element_interface->~IPoolElement();

                                // copy over the deleted element's data with last element's data
                                memcpy(deleted_element_mem, last_element_mem, element_size);
                                component_random_access_pools.m_element_isactives[pool_index][deleted_element_index] =
                                    component_random_access_pools.m_element_isactives[pool_index][last_element_index];
                                component_random_access_pools.m_element_isactives[pool_index][last_element_index] = false;
                                // get the handle of the last element that is copying over the deleted element
                                pool_element_interface              = reinterpret_cast<IPoolElement*>(last_element_mem);
                                ComponentHandle last_element_handle = {pool_element_interface->m_pool_index,
                                                                       pool_element_interface->m_redirection_index};

                                // set the last element's redirection value to the deleted element's redirection value
                                component_random_access_pools
                                    .m_redirection_indices[pool_index][last_element_handle.redirection_index] =
                                    component_random_access_pools.m_redirection_indices[pool_index][deleted_redirection_index];

                                pool_element_interface->m_pool_index =
                                    component_random_access_pools.m_redirection_indices[pool_index][deleted_redirection_index];

                                component_random_access_pools.m_redirection_indices[pool_index][deleted_redirection_index] = -1;

                                last_element_index--;
                        }
                        component_random_access_pools.m_element_counts[pool_index] = last_element_index + 1;
                }

                // get redirection_index for allocation
                Allocation Allocate(RandomAccessPools& component_random_access_pools, index pool_index)
                {
                        index next_free = component_random_access_pools.m_free_redirection_indices[pool_index].top();
                        component_random_access_pools.m_free_redirection_indices[pool_index].pop();

                        index   element_index       = component_random_access_pools.m_element_counts[pool_index];
                        component_random_access_pools.m_redirection_indices[pool_index][next_free] = element_index;

                        byte*   elemement_mem_start = component_random_access_pools.m_mem_starts[pool_index];
                        memsize elemement_size      = component_random_access_pools.m_elment_byte_sizes[pool_index];
                        byte*   element_mem         = elemement_mem_start + element_index * elemement_size;

                        component_random_access_pools.m_element_isactives[pool_index][element_index] = true;

                        component_random_access_pools.m_element_counts[pool_index]++;

						// it is important that this assert is <= and not <
                        assert(component_random_access_pools.m_element_counts[pool_index] <=
                               component_random_access_pools.m_element_capacities[pool_index]);

                        return {next_free, element_mem};
                }

                void ReleaseRedirectionIndices(RandomAccessPools& component_random_access_pools,
                                               index              pool_index,
                                               indices&           redirection_indices)
                {
                        for (index i = 0; i < redirection_indices.size(); i++)
                        {
                                if (component_random_access_pools.m_redirection_indices[pool_index][redirection_indices[i]] ==
                                    -1)
                                        component_random_access_pools.m_free_redirection_indices[pool_index].push(
                                            redirection_indices[i]);
                        }
                }
        } // namespace NPools
} // namespace NMemory