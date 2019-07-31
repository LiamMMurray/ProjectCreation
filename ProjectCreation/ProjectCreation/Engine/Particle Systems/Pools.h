#pragma once
#include <stdint.h>
namespace Pools
{
        template <typename T, int16_t N>
        class sorted_pool_t
        {
            public:
                // Todo: Implement the function bodies

                // Returns the number of active elements
                size_t size() const
                {
                        return active_count;
                }

                // Returns the maximum supported number of elements
                size_t capacity() const
                {
                        return N;
                }

                // Returns the value at the specified index
                T& operator[](int16_t index)
                {
                        return pool[index];
                }

                // Returns the value at the specified index
                const T& operator[](int16_t index) const
                {
                        return pool[index];
                }

                // Returns the index of the first inactive element
                //   and updates the active count
                // Returns -1 if no inactive elements remain
                static int16_t alloc()
                {
                        if (active_count >= N)
                                return -1;

                        return active_count++;
                }

                // Moves the element at 'index' to the inactive
                // region and updates the active count
                void free(int16_t index)
                {
                        std::swap(pool[index], pool[--active_count]);
                }

            private:
                T pool[N];

                int16_t active_count = 0;
        };

        template <typename T, int16_t N>
        class pool_t
        {
            public:
                // Todo: Implement the function bodies

                // Removes the first element from the free list and returns its index
                // Returns -1 if no free elements remain
                int16_t alloc()
                {
                        if (free_start == -1)
                                return -1;
                        int16_t index = free_start;
                        free_start    = pool[index].next;
                        return index;
                }

                // Adds 'index' to the free list
                void free(int16_t index)
                {
                        pool[index].next = free_start;
                        free_start       = index;
                }

                // Initializes the free list
                pool_t()
                {
                        for (int i = 0; i < N - 1; i++)
                        {
                                pool[i].next = i + 1;
                        }
                        pool[N - 1].next = -1;
                }

                // Returns the value at the specified index
                T& operator[](int16_t index)
                {
                        return pool[index].value;
                }

                // Returns the value at the specified index
                const T& operator[](int16_t index) const
                {
                        return pool[index].value;
                }

            private:
                union element_t
                {
                        T       value;
                        int16_t next;
                };

                element_t pool[N];

                int16_t free_start = 0;
        };

        template <typename T>
        class dynamic_pool_t
        {
            public:
                // Todo: Implement the function bodies
                void Init(size_t _size)
                {
                        delete[] pool;
                        pool = new element_t[_size];
                        size = _size;

                        for (int i = 0; i < size - 1; i++)
                        {
                                pool[i].next = i + 1;
                        }
                        pool[size - 1].next = -1;
                }
				
				dynamic_pool_t() = default;

                ~dynamic_pool_t()
                {
                        delete[] pool;
                }

                // Removes the first element from the free list and returns its index
                // Returns -1 if no free elements remain
                int16_t alloc()
                {
                        if (free_start == -1)
                                return -1;
                        int16_t index = free_start;
                        free_start    = pool[index].next;
                        return index;
                }

                // Adds 'index' to the free list
                void free(int16_t index)
                {
                        pool[index].next = free_start;
                        free_start       = index;
                }

                // Initializes the free list

                // Returns the value at the specified index
                T& operator[](int16_t index)
                {
                        return pool[index].value;
                }

                // Returns the value at the specified index
                const T& operator[](int16_t index) const
                {
                        return pool[index].value;
                }

            private:
                struct element_t
                {
                        T       value;
                        int16_t next;
                };

                element_t* pool = nullptr;

                size_t  size;
                int16_t free_start = 0;
        };
} // namespace Pools