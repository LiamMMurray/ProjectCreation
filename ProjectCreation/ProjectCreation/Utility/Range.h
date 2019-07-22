#pragma once
#include "../ECS/Memory.h"
template <typename T>
class range_iterator;

template <typename T>
class range
{
    public:
        friend class range_iterator<T>;
        typedef range_iterator<T> iterator;
        typedef ptrdiff_t         difference_type;
        typedef size_t            size_type;
        typedef T                 value_type;
        typedef T*                pointer;
        typedef T&                reference;

    private:
        size_type size;
        pointer   data;

    public:
        range(pointer data, size_type size) : data(data), size(size)
        {}

        iterator begin()
        {
                return iterator(*this, 0);
        }

        iterator end()
        {
                return iterator(*this, size);
        }

        reference operator[](size_type index)
        {
                assert(index < size);
                return data[index];
        }
};

template <typename T>
class range_iterator
{
    private:
        range<T>& _range;
        ptrdiff_t current_offset;

    public:
        range_iterator(range<T>& _range, ptrdiff_t _offset) : _range(_range), current_offset(_offset)
        {}
        bool operator==(range_iterator<T> other) const
        {
                return this->current_offset == other.current_offset;
        }
        bool operator!=(range_iterator<T> other) const
        {
                return this->current_offset != other.current_offset;
        }
        T& operator*()
        {
                return _range[current_offset];
        }
        range_iterator& operator++()
        {
                ++current_offset;
                return *this;
        }
        range_iterator operator++(int)
        {
                range_iterator clone(_range, current_offset);
                ++current_offset;
                return clone;
        }
};


template <typename T>
class active_range_iterator;

template <typename T>
class active_range
{
    public:
        friend class active_range_iterator<T>;
        typedef active_range_iterator<T> iterator;
        typedef ptrdiff_t                difference_type;
        typedef size_t                   size_type;
        typedef T                        value_type;
        typedef T*                       pointer;
        typedef T&                       reference;

    private:
        size_type                size;
        pointer                  data;
        NMemory::dynamic_bitset& isActives;

    public:
        active_range(pointer data, size_type size, NMemory::dynamic_bitset& isActives) :
            data(data),
            size(size),
            isActives(isActives)
        {}
        active_range(pointer data, size_type size, NMemory::dynamic_bitset&& isActives) :
            data(data),
            size(size),
            isActives(isActives)
        {}
        static active_range<T> SGetNullActiveRange()
        {
                return active_range(0, 0, NMemory::dynamic_bitset());
        }
        iterator begin()
        {
                return iterator(*this, 0);
        }

        iterator end()
        {
                return iterator(*this, size);
        }

        reference operator[](size_type index)
        {
                assert(index < size);
                return data[index];
        }
};

template <typename T>
class active_range_iterator
{
    private:
        active_range<T>& _range;
        size_t           current_offset;

    public:
        active_range_iterator(active_range<T>& _range, size_t _offset) : _range(_range), current_offset(_offset)
        {
                if (_offset < _range.size && !_range.isActives[_offset])
                        operator++();
        }
        bool operator==(active_range_iterator<T> other) const
        {
                return this->current_offset == other.current_offset;
        }
        bool operator!=(active_range_iterator<T> other) const
        {
                return this->current_offset != other.current_offset;
        }
        T& operator*()
        {
                return _range[current_offset];
        }
        active_range_iterator& operator++()
        {
                do
                {
                        current_offset++;
                } while (current_offset < _range.size && !_range.isActives[current_offset]);
                return *this;
        }
        active_range_iterator operator++(int)
        {
                active_range_iterator clone(_range, current_offset);
                do
                {
                        ++current_offset;
                } while (current_offset < _range.size && !_range.isActives[current_offset]);
                return clone;
        }
};
