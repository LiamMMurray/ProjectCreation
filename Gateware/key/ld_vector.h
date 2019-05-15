/************************************************
	Lazy Delete Vector

	description :

	the lazy delete vector is a vector. or more to the point,
	it can emulate a vector's behavior. this however is not it's
	purpose, merely a side benefit. The ld_vector is a factory
	design pattern transparent to the user.	Use the ld_vector
	when the ordering of your data is unimportant, but speed is.

	Author : Lari H. Norri
	date started : 10/??/04
	last modified : 01/06/19 (in FINAL rev. 1.8)

	Use :	for exact "stl vector" behavior, do not use any of the following
			functions. "remove, or "set_valid".

			for "factory (FAST!!!)" behavior, do not use the following:
			"for loops with array indexing"(use the provided iterator instead),
			"erase", "insert", "collapse_left", or "collapse_right"

			you can mix these behaviors safely, but read the function comments to
			understand the reprucusions. I provide some vector mimicing operations
			for flexibility and easy code integration, however this class is designed
			for superfast random access + adding + deleteing. I do not recommend this
			class if the order of your data is very important to you.

	Licence :	This code is free to use/modify whatever.
				Just mention me as the original author.

	Thanks :	A very big THANK YOU goes out to Lee Wood,
				your knowledge and insight have been invaluable
				during the development of this ADT. I would	also
				like to thank Chris Ragland and Nelson Nievies
				for their input. Thanks to Jeff Jackowski for
				adding in-place constructors and destructors
				for better compatibility with complex classes. 
				finally Hewlet Packard for your amazing STL.

***********************************************/

#ifndef _LD_VECTOR_H_
#define _LD_VECTOR_H_

#include <assert.h>
#include <memory.h>
#include <new>   // needed for in-place new

// defines an signed/unsigned interger 32 bits in length
// and a boolean value 8 bits in length
#define uInt32 unsigned int
#define Int32 signed int
#define Bool8 bool

// Levels of bit division for 32. 32^1, 32^2, 32^3...
#define LOW_SHIFT	>>5u
#define KI_SHIFT	>>10u
#define KI32_SHIFT	>>15u
#define MI_SHIFT	>>20u
#define MI32_SHIFT	>>25u

// the class is a little heavier now at 68 bytes pre-push_back.
// however, ld_vector will now retain maximum efficiency up to 33.5 million items
// after that, search time will increase by 1 cycle per every extra 33.5 million
template< typename T >
class ld_vector
{
	// how much can you hold vs what your holding.
	uInt32 _capacity, _size, _last;	// last is defined as one past the end

	// bit maps	(used for fast adding/removal)
	uInt32 *Mi32_map,	Mi32_sz;    // 32 bits per 33554432 items, 32^5 bits tracked
	uInt32 *Mi_map,		Mi_sz;     // 32 bits per 1048576 items, 32^4 bits tracked
	uInt32 *Ki32_map,	Ki32_sz;  // 32 bits per 32768 items, 32^3 bits tracked
	uInt32 *Ki_map,		Ki_sz;   // 32 bits per 1024 items, 32^2 bits tracked
	uInt32 *low_map,	low_sz; // 32 bits per 32 items, 32^1 bits tracked

	// all the template data stored by the vector
	T *Data;

	// stores up to date accurate info about this ld_vector
	struct Arbitrator
	{
		uInt32 *_end; // one past the last item
		uInt32 *lmap; // pointer to the active lowmap (for iteration)
		T *dat;	// data vault access
	};
	Arbitrator arbiter;

	// Macros for enabling, disabling and testing bits in the maps (TEST_MAP_BIT is only used by the low_map)
	#define TEST_MAP_BIT(map, index) (((map)[(index) >> 5u]) & (0x00000001u << ((index) & 31u)))
	// map = map to shift, div = divisor for the index (these macros turn bits on & off in an array)
	#define SHIFT_ENABLE(map,index) (map)[(index)>>5u] |= (0x00000001u << ((index) & 31u))
	#define SHIFT_DISABLE(map,index) (map)[(index)>>5u] &= ~(0x00000001u << ((index) & 31u))
	// enables a bit in the low map and any subsequent higher map
	#define ENABLE_MAP_BIT(index) \
			SHIFT_ENABLE(low_map,(index));\
			if(0xFFFFFFFF == low_map[(index)LOW_SHIFT]){\
				SHIFT_ENABLE(Ki_map,(index)LOW_SHIFT);\
				if(0xFFFFFFFF == Ki_map[(index)KI_SHIFT]){\
					SHIFT_ENABLE(Ki32_map,(index)KI_SHIFT);\
					if(0xFFFFFFFF == Ki32_map[(index)KI32_SHIFT]){\
						SHIFT_ENABLE(Mi_map,(index)KI32_SHIFT);\
						if(0xFFFFFFFF == Mi_map[(index)MI_SHIFT]){\
							SHIFT_ENABLE(Mi32_map,(index)MI_SHIFT);}}}}

	// disables a bit in the low map and any subsequent higher map
	#define DISABLE_MAP_BIT(index) \
			SHIFT_DISABLE(low_map,(index));\
			if(0xFFFFFFFF != low_map[(index)LOW_SHIFT]){\
				SHIFT_DISABLE(Ki_map,(index)LOW_SHIFT);\
				if(0xFFFFFFFF != Ki_map[(index)KI_SHIFT]){\
					SHIFT_DISABLE(Ki32_map,(index)KI_SHIFT);\
					if(0xFFFFFFFF != Ki32_map[(index)KI32_SHIFT]){\
						SHIFT_DISABLE(Mi_map,(index)KI32_SHIFT);\
						if(0xFFFFFFFF != Mi_map[(index)MI_SHIFT]){\
							SHIFT_DISABLE(Mi32_map,(index)MI_SHIFT);}}}}

	// the following macros are used to find an open bit quickly from a 32bit value
	#define _TEST_IF(x, b32) if((x) & ~(b32)) {
	#define _TEST_ELSE(x, b32) } else if((x) & ~(b32)) {
	#define _SET_IF(x, b32, t, f, i) (i) = ((x) & ~(b32)) ? (t) : (f);
	#define _SET_ELSE(x, b32, t, f, i) } else if((x) & ~(b32)) { (i) = (t); } else { (i) = (f); }

	// finds an open bit(index) from a 32 bit value (using divide & conquer)
	// btw... yes i know this is a flagrant abuse of macros,
	// but you really dont want to see this unrolled.
	#define FIND_OPEN_BIT(i, b32) \
			{	_TEST_IF(0x0000FFFFu, b32)\
					_TEST_IF(0x000000FFu, b32)\
						_TEST_IF(0x0000000Fu, b32)\
							_TEST_IF(0x00000003u, b32)\
								_SET_IF(0x00000001u, b32, 0u, 1u, i)\
							_SET_ELSE(0x00000004u, b32, 2u, 3u, i)\
						_TEST_ELSE(0x00000030u, b32)\
							_SET_IF(0x00000010u, b32, 4u, 5u, i)\
						_SET_ELSE(0x00000040u, b32, 6u, 7u, i)\
					_TEST_ELSE(0x00000F00u, b32)\
						_TEST_IF(0x00000300u, b32)\
							_SET_IF(0x00000100u, b32, 8u, 9u, i)\
						_SET_ELSE(0x00000400u, b32, 10u, 11u, i)\
					_TEST_ELSE(0x00003000u, b32)\
						_SET_IF(0x00001000u, b32, 12u, 13u, i)\
					_SET_ELSE(0x00004000u, b32, 14u, 15u, i)\
				_TEST_ELSE(0x00FF0000u, b32)\
					_TEST_IF(0x000F0000u, b32)\
						_TEST_IF(0x00030000u, b32)\
							_SET_IF(0x00010000u, b32, 16u, 17u, i)\
						_SET_ELSE(0x00040000u, b32, 18u, 19u, i)\
					_TEST_ELSE(0x00300000u, b32)\
						_SET_IF(0x00100000u, b32, 20u, 21u, i)\
					_SET_ELSE(0x00400000u, b32, 22u, 23u, i)\
				_TEST_ELSE(0x0F000000u, b32)\
					_TEST_IF(0x03000000u, b32)\
						_SET_IF(0x01000000u, b32, 24u, 25u, i)\
					_SET_ELSE(0x04000000u, b32, 26u, 27u, i)\
				_TEST_ELSE(0x30000000u, b32)\
					_SET_IF(0x10000000u, b32, 28u, 29u, i)\
				_SET_ELSE(0x40000000u, b32, 30u, 31u, i)\
			}

	// this is called by any constructor to initialize the vector
	void _create(void)
	{
		// the vector can hold nothing by default
		_capacity = _size =
		// all bit maps start with 0 bits
		Mi32_sz = Mi_sz = Ki32_sz = Ki_sz =	low_sz = 0u;
		// all maps are safe to delete initially
		Mi32_map = Mi_map = Ki32_map = Ki_map = low_map = 0u;
		Data = 0u;	// clear all data
		_last = 0u;	// the index of the last element + 1
		// zero out the arbiter
		memset(&arbiter, 0u, sizeof(Arbitrator));
		arbiter._end = &_last; // fixes crashing when iterating on an empty list
	}

	// find all allocated objects and destroy them
	void _destruct(void) {
		iterator i = begin();
		while (i != end()) {
			(*i).~T();
			i++;
		}
	}

	// when the vector runs out of memory, this command doubles its capacity.
	// This function could be changed to expand in a single step to the required
	// capacity so that resizing from 0 to pow(2, n) doesn't take n calls of
	// this function.
	void _expand(void)
	{
		// double the capacity
		(_capacity) ? _capacity = _capacity << 1u : _capacity = 1u;
		T * old = Data;
		Data = (T*)malloc(sizeof(T) *_capacity);  // allocate new data array

		// macro will double & retain the contents of an uInt32 array
		#define _GROW_ARRAY(arr,sz) \
				{	(sz) ? sz = sz << 1u : sz = 1u;\
					uInt32 * _narr = new uInt32[sz];\
					memset(_narr, 0u, sz << 2u);\
					if(sz > 1u) memcpy(_narr, arr, sz << 1u);\
					delete [] arr;\
					arr = _narr;\
				}

		// allocate the bit-maps if neccasary
		if(!low_sz || low_sz < (_capacity LOW_SHIFT)) _GROW_ARRAY(low_map,low_sz);
		if(!Ki_sz || Ki_sz < (_capacity KI_SHIFT)) _GROW_ARRAY(Ki_map,Ki_sz);
		if(!Ki32_sz || Ki32_sz < (_capacity KI32_SHIFT)) _GROW_ARRAY(Ki32_map,Ki32_sz);
		if(!Mi_sz || Mi_sz < (_capacity MI_SHIFT)) _GROW_ARRAY(Mi_map,Mi_sz);
		if(!Mi32_sz || Mi32_sz < (_capacity MI32_SHIFT)) _GROW_ARRAY(Mi32_map,Mi32_sz);

		// don't need the macro anymore
		#undef _GROW_ARRAY

		// update the arbiter (this is the only time we need do so)
		arbiter._end = &_last;
		arbiter.dat = Data;
		arbiter.lmap = low_map;

		// move data (v1.7 fixed some 64bit compatibility issues (more to come))
		iterator i = begin();
		while (i != end()) { // v1.8 returned old C style casts for compatibility
			new (Data + (uInt32)i) T(old[(uInt32)i]); // copy construct new data
			old[(uInt32)i].~T(); // destruct old data
			i++;
		}
		free(old);  // deallocate old data
	}

public:

	// define the nested iterator class
	class iterator;
	friend class iterator;

	class iterator
	{
		uInt32 index; // our current index into the ld_vector
		const Arbitrator *envoy; // lets the iter stay informed about it's designated vector

		public :

		// creates an iterator with the proper values
		explicit iterator(uInt32 i = 0u, const Arbitrator *a = 0u) : index(i), envoy(a) { }

		T *operator->() const { return &envoy->dat[index]; }
		T &operator*() const { return envoy->dat[index]; }

		Bool8 operator==(const iterator &comp) const
		{	return index == comp.index && envoy == comp.envoy;	}

		Bool8 operator!=(const iterator &comp) const
		{	return index != comp.index || envoy != comp.envoy;	}

		// coverts an iterator directly to it's index
		operator uInt32() const { return index; }
		operator Int32() const { return (Int32)index; }
		// is false if the iterator is not at a valid location
		operator Bool8() const
		{
			// take range into account
			if(!envoy->_end || index >= *envoy->_end) return false;
			return (TEST_MAP_BIT(envoy->lmap,index)) ? true : false;
		}

		iterator &operator++()
		{
			if(index+1u >= (*envoy->_end)+1u) return (*this); // don't iterate after the end
			while(++index < (*envoy->_end) && !TEST_MAP_BIT(envoy->lmap, index))
				if(!(index & 31u) && 0x00000000u == envoy->lmap[index >> 5u])
					if(index + 32u < (*envoy->_end)) index += 31u; // skip 32 empty items if possible
			return (*this);
		}

		iterator &operator--()
		{
			// if you reverse iterate on begin(0) it will take you to end(_last)
			if(!index) { index = *envoy->_end; return (*this); }
			while(--index && !TEST_MAP_BIT(envoy->lmap, index)) // hmmm
				if(index > 31u && !((index-32u) & 31u) && 0x00000000u == envoy->lmap[(index-32u) >> 5u])
					index -= 31u; // skip 32 empty items if possible
			return (*this);
		}

		iterator operator++(Int32)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		iterator operator--(Int32)
		{
			iterator tmp = *this;
			--*this;
			return tmp;
		}

		iterator operator+(Int32 mov) const { return iterator(index + mov, envoy); }
		iterator operator-(Int32 mov) const { return iterator(index - mov, envoy); }
		iterator operator+(uInt32 mov) const { return iterator(index + mov, envoy); }
		iterator operator-(uInt32 mov) const { return iterator(index - mov, envoy); }
	};

	// define the nested const_iterator class
	// exactly like the first one, but const
	class const_iterator;
	friend class const_iterator;
	// the only difference between the iterators, is that you may not edit the values of the const iter
	class const_iterator
	{
		uInt32 index; // our current index into the ld_vector
		const Arbitrator *envoy; // lets the iter stay informed about it's designated vector
	public :
		// creates an const_iterator with the proper values
		explicit const_iterator(uInt32 i = 0u, const Arbitrator *a = 0u) : index(i), envoy(a) { }
		// data accessors (const)
		const T *operator->() const { return &envoy->dat[index]; }
		const T &operator*() const { return envoy->dat[index]; }
		// comparison ops
		Bool8 operator==(const const_iterator &comp) const
		{	return index == comp.index && envoy == comp.envoy;	}
		Bool8 operator!=(const const_iterator &comp) const
		{	return index != comp.index || envoy != comp.envoy;	}
		// coverts an const_iterator directly to it's index
		operator uInt32() const { return index; }
		operator Int32() const { return (Int32)index; }
		// is false if the const_iterator is not at a valid location
		operator Bool8() const
		{
			// take range into account
			if(!envoy->_end || index >= *envoy->_end) return false;
			return (TEST_MAP_BIT(envoy->lmap,index)) ? true : false;
		}
		// increment op
		const_iterator &operator++()
		{
			if(index+1u >= (*envoy->_end)+1u) return (*this); // don't iterate after the end
			while(++index < (*envoy->_end) && !TEST_MAP_BIT(envoy->lmap, index))
				if(!(index & 31u) && 0x00000000u == envoy->lmap[index >> 5u])
					if(index + 32u < (*envoy->_end)) index += 31u; // skip 32 empty items if possible
			return (*this);
		}
		// decrement op
		const_iterator &operator--()
		{
			// if you reverse iterate on begin(0) it will take you to end(_last)
			if(!index) { index = *envoy->_end; return (*this); }
			while(--index && !TEST_MAP_BIT(envoy->lmap, index)) // hmmm
				if(index > 31u && !((index-32u) & 31u) && 0x00000000u == envoy->lmap[(index-32u) >> 5u])
					index -= 31u; // skip 32 empty items if possible
			return (*this);
		}
		// increment post
		const_iterator operator++(Int32)
		{
			const_iterator tmp = *this;
			++*this;
			return tmp;
		}
		// decrement post
		const_iterator operator--(Int32)
		{
			const_iterator tmp = *this;
			--*this;
			return tmp;
		}
		// jump by element amount
		const_iterator operator+(Int32 mov) const { return const_iterator(index + mov, envoy); }
		const_iterator operator-(Int32 mov) const { return const_iterator(index - mov, envoy); }
		const_iterator operator+(uInt32 mov) const { return const_iterator(index + mov, envoy); }
		const_iterator operator-(uInt32 mov) const { return const_iterator(index - mov, envoy); }
	};

	// constructor + trilogy of evil
	ld_vector(void) { _create(); } // default constructor
	ld_vector(uInt32 count) { _create(); reserve(count); } // this constructor reserves room
	ld_vector(uInt32 count, const T &val) // this reserves and sets all items to "val"
	{
		_create(); reserve(count);
		for(uInt32 i = 0; i < count; ++i)
		{ new (Data + i) T(val); ENABLE_MAP_BIT(i); }
		_size = count;
	}

	// assigment operator
	ld_vector<T> &operator=(const ld_vector<T> &cpy)
	{
		// don't perform needless copy
		if(this == &cpy) return *this;
		// copy the valid contents of the incomming vector
		clear(); // empty this vector
		// reserve room	to copy to
		if(_capacity < cpy._capacity) reserve(cpy._capacity);
		// copy all valid items
		const_iterator copy = cpy.begin(), last = cpy.end();
		while(copy != last)
		{
			new (Data + (uInt32)copy) T(cpy.Data[(uInt32)copy]);
			ENABLE_MAP_BIT((uInt32)copy);
			++copy;
		}
		// match the sizes & the last item
		_size = cpy._size;
		_last = cpy._last;
		return *this;
	}

	// copy constructor
	ld_vector(const ld_vector<T> &copy) { _create(); *this = copy; }

	// destructor
	~ld_vector(void) { _destroy(); }

	// return an iterator to the front of the ld_vector
	iterator begin()
	{
		return (_capacity) ? ++iterator(0xFFFFFFFFu, &arbiter) : iterator(0u, &arbiter);
	}
	const_iterator begin() const // const iterator version
	{
		return (_capacity)	? ++const_iterator(0xFFFFFFFFu, &arbiter)
							: const_iterator(0u, &arbiter);
	}
	// return an iterator to one past the end of the ld_vector
	iterator end()
	{
		return iterator(_last, &arbiter);
	}
	const_iterator end() const // const iterator version
	{
		return const_iterator(_last, &arbiter);
	}

	// returns the number of items in the vector
	uInt32 size(void) const { return _size; }
	// returns the capacity of the vector
	uInt32 capacity(void) const { return _capacity; }

	// reserves at least r items in the vector (memory is always reserved in powers of two)
	// capacity	is garunteed to be >= to r
	void reserve(uInt32 r) { while(_capacity < r) _expand(); } // could be optimized

	// return an item from the array
	T &operator[](uInt32 index) {
		// the index must be within bounds
		assert(index < _capacity);
		// the item must be allocated
		assert(TEST_MAP_BIT(low_map,index));
		return Data[index];
	}
	// return an item from the array const
	const T &operator[](uInt32 index) const {
		// the index must be within bounds
		assert(index < _capacity);
		// the item must be allocated
		assert(TEST_MAP_BIT(low_map,index));
		return Data[index];
	}

	// tells you if an index in the array is in use
	Bool8 is_valid(uInt32 index) const
	{
		if(index >= _capacity) return false;
		return (TEST_MAP_BIT(low_map,index)) ? true : false;
	}

	// forces an index to make itself valid/Invalid
	// making an index Invalid is the same as calling remove on an iterator at the same index.
	void set_valid(uInt32 index, Bool8 valid = true)
	{
		if(valid && !TEST_MAP_BIT(low_map, index))
		{
			ENABLE_MAP_BIT(index); ++_size;
			if(index >= _last) _last = index+1u; // possibly update last position
			new (Data + index) T;  // construct the item
		}
		else if(!valid && TEST_MAP_BIT(low_map, index))
		{
			if(index == _last-1u) --_last; // possibly update last position
			Data[index].~T();   // destruct the item
			DISABLE_MAP_BIT(index); --_size;
		}
	}

	// returns an open slot in the vector, if none it will expand
	// this function is VOLATILE!
	// Meaning the vector may be in a different memory pool after you call it. (_expand)
	uInt32 find_open(void)
	{
		// if the vector is full, expand it
		if(_size >= _capacity) _expand();
		// now... an open slot is garunteed	so lets find it.
		uInt32 shift = 0u, _bit;
		while(Mi32_map[shift] == 0xFFFFFFFFu) ++shift; // move to an open 33.5 million
		// from here use divide & conquer to find an open location as fast as possible
		if(Mi32_map[shift]) { FIND_OPEN_BIT(_bit, Mi32_map[shift]); } else _bit = 0u;
		shift = (shift << 5u) + _bit; // first open million
		if(Mi_map[shift]) { FIND_OPEN_BIT(_bit, Mi_map[shift]); } else _bit = 0u;
		shift = (shift << 5u) + _bit; // first open 32 thousand
		if(Ki32_map[shift]) { FIND_OPEN_BIT(_bit, Ki32_map[shift]); } else _bit = 0u;
		shift = (shift << 5u) + _bit; // first open 1024
		if(Ki_map[shift]) { FIND_OPEN_BIT(_bit, Ki_map[shift]); } else _bit = 0u;
		shift = (shift << 5u) + _bit; // first open 32
		if(low_map[shift]) { FIND_OPEN_BIT(_bit, low_map[shift]); } else _bit = 0u;
		return (shift << 5u) + _bit; // the open item
	}

	// adds an item to the vector and returns its index (index is garunteed to stay valid)
	// the only operations that can effect indicies	are: collapse_left, collapse_right, insert, and erase.
	// keep in mind that push_back fills unused slots first, only then does it add to the end.
	// this function is VOLATILE!
	// Meaning the vector may be in a different memory pool after you call it. (_expand)
	uInt32 push_back(const T &val)
	{
		// IMPORTANT NOTE: Never use push_back in the following manner
		// "myvector[x].val = myvector.push_back( any )"
		// Instead use : "int loc = myvector.push_back( any ); myvector[x].val = loc;"
		// Although this usage is rare...
		// Keep in mind that a vector can and will switch internal memory banks as required.
		uInt32 _open = find_open();
		new (Data + _open) T(val);
		ENABLE_MAP_BIT(_open);
		++_size;
		if(_open >= _last) _last = _open+1u; // update the _last spot
		return _open;
	}

	// Insert an element before a specific point
	// returns an iterator to the newly inserted element
	// this will distort indicies, use "push_back" if you want to maintain index intergrity.
	iterator insert(iterator point, const T &val)
	{
		// if the vector is full, or it cannot expand right, expand it
		iterator shift = --end();
		if(_size >= _capacity || ((uInt32)shift) == _capacity-1u) _expand();
		// shift all items right one spot
		while(shift != point)
		{
			new (Data + (uInt32)shift+1u) T(Data[(uInt32)shift]);
			Data[(uInt32)shift].~T();
			DISABLE_MAP_BIT((uInt32)shift);
			ENABLE_MAP_BIT(((uInt32)shift)+1u);
			--shift;
		}
		// shift current item in point
		new (Data + (uInt32)point+1u) T(Data[(uInt32)point]);
		Data[(uInt32)point].~T();
		ENABLE_MAP_BIT((uInt32)point + 1u);
		// add item
		new (Data + (uInt32)point) T(val); ++_size;
		// move the _last position, one to the right
		++_last;
		return point;
	}

	// removes the last valid item in the vector
	void pop_back(void)
	{
		if(!_size) return;
		set_valid((uInt32)--end(), false);
	}

	// return refrences to the first and last valid items.
	T &front(void) { return *begin(); }
	const T &front(void) const { return *begin(); }
	T &back(void) { return *(--end()); }
	const T &back(void) const { return *(--end()); }

	// removes an item from the vector (does not effect other items)
	// returns an iterator to the next valid item
	iterator remove(iterator rem) { set_valid(rem, false); return ++rem; }

	// removes all gaps from the vector, and lines data starting from the begining
	void collapse_left(void)
	{
		if(!_size || _size == _capacity) return;
		uInt32 track = 0u; // when track == _size, all items have been shifted left
		iterator find; // quickly find an item in use
		while(track < _size)
		{
			if(!TEST_MAP_BIT(low_map, track)) // determine if an empty spot must be filled
			{
				find = ++iterator(track, &arbiter); // find used item
				new (Data + track) T(Data[(uInt32)find]); // fill the open slot with a valid item
				Data[(uInt32)find].~T();
				// update the bitmaps
				DISABLE_MAP_BIT((uInt32)find)
				ENABLE_MAP_BIT(track)
			}
			++track; // move to the next item
		}
		// update the last point
		_last = ((uInt32)(--iterator(_capacity,&arbiter)))+1u;
	}

	// removes all gaps from the vector, and lines data starting from the end
	void collapse_right(void)
	{
		if(!_size || _size == _capacity) return;
		uInt32 track = _capacity-1u; // when track < _capacity - _size, all items have been shifted right
		iterator find; // quickly find an item in use
		while(track >= (_capacity - _size))
		{
			if(!TEST_MAP_BIT(low_map, track)) // determine if an empty spot must be filled
			{
				find = --iterator(track, &arbiter); // find used item
				new (Data + track) T(Data[(uInt32)find]); // fill the open slot with a valid item
				Data[(uInt32)find].~T();
				// update the bitmaps
				DISABLE_MAP_BIT((uInt32)find)
				ENABLE_MAP_BIT(track)
			}
			--track; // move to the prev item
		}
		// update the last point
		_last = _capacity;
	}

	// removes an item from the vector and collapses all items after it to the left
	// this will distort indicies, use "remove" if you want to maintain index intergrity.
	iterator erase(iterator ers)
	{
		// now collapse all following items
		if(!_size) return end();
		uInt32 track = (uInt32)ers;
		iterator find = ers; // quickly find an item in use
		iterator last = --end(); // last valid item in the list
		set_valid(ers, false); // gets rid of the item
		if(ers == last) return end();
		while(find != last)
		{
			if(!TEST_MAP_BIT(low_map, track)) // determine if an empty spot must be filled
			{
				++find;// = ++iterator(track, &arbiter); // find used item
				new (Data + track) T(Data[(uInt32)find]); // fill the open slot with a valid item
				Data[(uInt32)find].~T();
				// update the bitmaps
				DISABLE_MAP_BIT((uInt32)find)
				ENABLE_MAP_BIT(track)
			}
			++track; // move to the next item
		}
		// update last point
		_last = track;
		return ers;
	}

	// clear the vector of all elements but do not effect its capacity
	void clear(void)
	{
		if(!_size) return;
		_destruct();
		_size = 0u;
		_last = 0u;
		memset(low_map, 0u, low_sz << 2u);
		memset(Ki_map, 0u, Ki_sz << 2u);
		memset(Ki32_map, 0u, Ki32_sz << 2u);
		memset(Mi_map, 0u, Mi_sz << 2u);
		memset(Mi32_map, 0u, Mi32_sz << 2u);
	}

	// clean out the vector of all elements and capacity
	void _destroy(void)
	{
		_destruct();
		free(Data);
		delete [] low_map;
		delete [] Ki_map;
		delete [] Ki32_map;
		delete [] Mi_map;
		delete [] Mi32_map;
		_create();
	}

};

// remove macros & defines
#undef uInt32
#undef Int32
#undef Bool8
#undef LOW_SHIFT
#undef KI_SHIFT
#undef KI32_SHIFT
#undef MI_SHIFT
#undef MI32_SHIFT
#undef TEST_MAP_BIT
#undef SHIFT_ENABLE
#undef SHIFT_DISABLE
#undef ENABLE_MAP_BIT
#undef DISABLE_MAP_BIT
#undef _TEST_IF
#undef _TEST_ELSE
#undef _SET_IF
#undef _SET_ELSE
#undef FIND_OPEN_BIT

#endif