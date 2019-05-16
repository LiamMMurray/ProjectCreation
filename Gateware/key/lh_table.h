/************************************************
	Lazy Hash Table

	description : 
	
	The lazy hash table is a simply a (seperate chaining)
	Hash table that uses a routing array to lookup/add new
	entries into the "ld_vector(see header file)" that
	stores the data used by the Lazy Hash Table.
	
	Author : Lari H. Norri
	date started : 06/04/07
	last modified : 07/31/09 (in Beta rev. 1.2)

	Use :	It's a hash table... if you don't know, GOOGLE IT!
			First provide a hashing function via the constructor.
			The result of your hash will be boundry limited for you.
			If none provided, a default hash operation will be used.
			Use "add" to add items to the hash table.
			Use "insert" to add an item and get back a const iterator to the new item.
			Use "remove" to [lazy delete] items from the table 
			Use "find" to determine wether an object exists or not.
			Use "clear" to dump the contents of the hash table but not the capacity
			The "==" operator must be usable by the template given

	License :	This code is free to use/modify whatever.
				Just mention me as the original author.
	
	Thanks :	Mike Barnoske & Mark Allen Weiss

***********************************************/

#ifndef _LH_TABLE_H_
#define _LH_TABLE_H_

// the ld_vector class is used to efficiently recycle data
#include "ld_vector.h"
#include <assert.h>

// define an unsigned/signed integer 32 bits in length
#define uInt32 unsigned int
#define Int32 signed int
// define a signed char 8 bits in length
#define Char8 signed char
// define boolean value 8bits in length
#define Bool8 bool

// converts x to the next pwr of 2
#define NEXT_PWR2(x)	(x) = (x) - 1u;\
						(x) = (x) | ((x) >> 1u);\
						(x) = (x) | ((x) >> 2u);\
						(x) = (x) | ((x) >> 4u);\
						(x) = (x) | ((x) >> 8u);\
						(x) = (x) | ((x) >>16u);\
						(x) = (x) + 1u

// tells if a value is a pwr of 2 or not
#define IS_PWR2(x)	(!(((x)-1u) & (x)))

// the lazy hash table
template<typename T>
class lh_table
{
	// size of this table's hashable locations (always a power of 2)
	uInt32 _tablesize; // will always be rounded up to the nearest pwr of 2
	// routing array
	uInt32 *_router; // an entry of 0xFFFFFFFF means an item is not in use
	// if you turn this flag on the hash table will allow duplicate data
	Bool8 _duplicate; 

	// Hash table node
	struct HASH_NODE { T _item; uInt32 _next; };
	// data storage
	ld_vector<HASH_NODE> _data;

	// internal hash function (ideally provided by user)
	uInt32 (*_hash)(const T&);

	// default hash function (discriminate bit-pattern hash)
	static uInt32 hash(const T &object)
	{
		uInt32 Ihash = sizeof(object); 
		Char8 *bytes = (Char8*)&object;
		for(uInt32 byte = 0u; byte < sizeof(object); ++byte)
			Ihash = (Ihash << 5u) ^ Ihash ^ bytes[byte];
		return Ihash;
	}

public:

	// define the nested iterator class (forward only)
	// the lh_table iterator allows you to "peek" at a bucket, but not change it.
	// if you wish to alter a table entry... use "find", "erase", "add" to do so 
	class iterator;
	friend class iterator;
	// iterator provides constant access to data in the hash table that is equal ==
	class iterator
	{
		// internal node iterator
		uInt32 _node;
		const ld_vector<HASH_NODE> *_pool;

	public:
		// constructor takes in a new node
		explicit iterator( uInt32 index = 0xFFFFFFFFu, const ld_vector<HASH_NODE> *nodes = 0u) 
							: _node(index), _pool(nodes) { }
		// accessing the data
		const T *operator->() const { return &((*_pool)[_node]._item); }
		const T &operator*() const { return (*_pool)[_node]._item; }
		// iterator comparison
		Bool8 operator==(const iterator &comp) const 
		{	return _node == comp._node && _pool == comp._pool;	}
		Bool8 operator!=(const iterator &comp) const 
		{	return _node != comp._node || _pool != comp._pool;	}
		// coverts an iterator directly to it's index
		operator uInt32() const { return _node; }
		operator Int32() const { return (Int32)_node; }
		// is this iterator valid?
		operator Bool8() const	{	return (~_node) ? true : false;	} 
		// iteration to the next matching item in the set
		iterator &operator++() 
		{ 
			uInt32 _eval = _node; // used for duplicate compare
			while(~_node) // don't iterate after the end
			{
				_node = (*_pool)[_node]._next;
				if(~_node && (*_pool)[_node]._item == (*_pool)[_eval]._item)
					break; // we found another one
			}
			return (*this); 
		}
		// post-increment op
		iterator operator++(Int32)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}
	};

	// define the nested data_iterator class
	// the data_iterator allows you to view at all the data in the table, but not change it.
	class data_iterator;
	friend class data_iterator;
	// data_iterator provides a way to run through all the data in the table very quickly (no order is garunteed)
	class data_iterator
	{
		// template pre-definition trick
		typedef typename ld_vector<HASH_NODE>::const_iterator const_iter;
		const_iter current;

	public:
		// constructor takes in a new node
		explicit data_iterator( const const_iter &loc ) 
			: current(loc) {}
		// accessing the data
		const T *operator->() const { return &current->_item; }
		const T &operator*() const { return current->_item; }
		// iterator comparison
		Bool8 operator==(const data_iterator &comp) const 
		{	return current == comp.current;	}
		Bool8 operator!=(const data_iterator &comp) const 
		{	return current != comp.current;	}
		// is this iterator valid?
		operator Bool8() const	{	return (Bool8)current;	} 
		// iteration to the next piece of data in the table
		data_iterator &operator++() 
		{ 
			++current;
			return (*this); 
		}
		// post-increment op
		data_iterator operator++(Int32)
		{
			data_iterator tmp = *this;
			++*this;
			return tmp;
		}
	};

	// constructor, initializes hash table size & hash function
	lh_table(	uInt32 min_table_size = 65536u, 
				uInt32(*hash_func)(const T&) = 0,
				Bool8 allow_duplicates = false)
	{
		_duplicate = allow_duplicates; // duplicate data is OFF by default
		_hash = (hash_func) ? hash_func : lh_table::hash; // set the default hasing op
		// minimum size is 128
		if(min_table_size < 128u) min_table_size = 128u;
		// ensure the table is sized as a pwr of 2. (this enhances our hash ability)
		if(!IS_PWR2(min_table_size)) { NEXT_PWR2(min_table_size); }
		_tablesize = min_table_size; // finnally assign the proper table size
		// allocate the hash table's router
		_router = new uInt32[_tablesize];
		// set the state of the routing table to unused entries
		memset(_router, 0xFFFFFFFF, _tablesize << 2);
	}
	// destructor frees routing table
	~lh_table() { delete [] _router; }
	// copy constructor & assigment operator
	lh_table(const lh_table &cpy)
	{
		_router = 0;
		*this = cpy;
	}
	lh_table &operator=(const lh_table &cpy)
	{
		if(this == &lh_table) return *this;
		delete [] _router;
		_router = new uInt32[cpy._tablesize];
		memcpy(_router, cpy._router, cpy._tablesize << 2);
		_duplicate = cpy.duplicate;
		_hash = cpy._hash;
		_data = cpy._data;
		return *this;
	}

	// returns the end of any hash table sequence (use this with the iterator in loops)
	iterator end() { return iterator( 0xFFFFFFFF, &_data ); }

	// returns an iterator to data at the start of the table
	data_iterator data_begin() const { return data_iterator(_data.begin()); }
	// return an iterator to the data at the end of the table
	data_iterator data_end() const { return data_iterator(_data.end()); }

	// count operation, tells you if an object exists in the hash table
	// returns 0 if none found otherwise returns 1 
	// if "_duplicate" is true it will return the count of matching objects
	uInt32 count(const T &obj) const
	{
		// count, bucket
		uInt32 c = 0u, b = _router[(_hash(obj) & (_tablesize-1u))];
		while(~b)
		{
			if(_data[b]._item == obj) 
			{	
				++c; // find count incremented
				if(!_duplicate) break; // don't search for more if you don't have to
			}
			b = _data[b]._next; // move to the next spot
		}
		return c;
	}

	// exists operation, fastest search, does not regard "_duplicate"
	// returns true if it found a match false otherwise 
	Bool8 exists(const T &obj) const
	{
		// bucket
		uInt32 f = _router[(_hash(obj) & (_tablesize-1u))];
		while(~f)
			if(_data[f]._item == obj) 
				return true;
			else f = _data[f]._next; // move to the next spot
		return false;
	}

	// find operation, will find the first occurance of "obj" in the hash table
	// if successfull, returns an valid iterator pointing to "obj" 
	// if "_duplicate" is enabled, you may use this iterator to find ALL "obj" occurances
	// if find fails or you iterate to far... the iterator will == "lh_table.end()"
	iterator find(const T &obj) const
	{
		// bucket
		uInt32 b = _router[(_hash(obj) & (_tablesize-1u))];
		while(~b)
			if(_data[b]._item == obj) 
				return iterator( b, &_data ); // found it! (duplicates follow)
			else
				b = _data[b]._next; // move to the next spot
		return iterator( b, &_data );// didn't find it
	}

	// add operation, adds an entry to the hash table
	// if "_duplicate" is true, it will add regardless of it already being there
	// returns if the item was added or not, true == item added, false == item exists already
	Bool8 add(const T &obj)
	{
		// add point & compute bucket
		uInt32 b = (_hash(obj) & (_tablesize-1u)), n = _router[b];
		// no duplicates allowed, look for a match
		if(!_duplicate)
		{
			while(~n) 
				if (_data[n]._item == obj)
					return false;
				else
					n = _data[n]._next;
		}
		// if we made it this far, add the new item to the front
		n = _data.find_open();
		_data.set_valid( n, true );
		_data[n]._item = obj;
		_data[n]._next = _router[b]; // link the chain
		_router[b] = n; // adds the new node to the table
		
		return true;	
	}

	// remove operation, removes the first occurance of an object
	// returns the number of items removed 0 == none remove
	// if you set "_all" to true, remove will ensure nothing matching "obj" is left
	uInt32 remove(const T &obj, Bool8 _all = false)
	{
		// count, bucket, prev pointer
		uInt32 c = 0u, *p = 0, b = (_hash(obj) & (_tablesize-1u)); 
		p = &_router[b]; // point to prev routing #
		b = _router[b]; // loop through hash entries
		while(~b)
		{
			if(_data[b]._item == obj) // delete this item? 
			{	
				*p = _data[b]._next;  // link the chain
				++c; // increment rmv count
				_data.set_valid( b, false ); // remove the _item
				if(!_all) break; // don't remove more if you don't have to
				b = *p; // move to the next item
			}
			else // this nodes next becomes the new previous
			{
				p = &_data[b]._next; // if not we don't edit the prevoius
				b = _data[b]._next; // move to the next item
			}
		} // return the # of items removed
		return c;
	}

	// erase operation, will specifically remove(only) the passed iterator from the hash table.
	// returns an iterator to the next duplicate after "ers" or "lh_table.end()" otherwise.
	// assumes "ers" is valid iterator to a hash table entry
	iterator erase( iterator ers )
	{
		// get the hash code
		uInt32 *p = 0u, b = (_hash(*ers) & (_tablesize-1u));
		p = &_router[b]; // point to prev routing #
		b = _router[b]; // loop through hash entries
		// is this bucket equivilant to ers?
		while(~b && b != (uInt32)ers)
		{
			p = &_data[b]._next; // edit the prevoius
			b = _data[b]._next; // move to the next item
		}
		if(~b)// if we found it... kill it
		{
			++ers; // shift to next possible duplicate	
			*p = _data[b]._next;  // link the chain
			_data.set_valid( b, false ); // remove the _item
			return ers; // return next in line
		}
		return end(); // not found
	}
	
	// performs the same behavior as add, but returns an iterator
	// to the new item. If unable to add the new item, the resulting
	// iterator will be equal to == lh_table.end()
	iterator insert(const T &obj)
	{
		// add point & compute bucket
		uInt32 b = (_hash(obj) & (_tablesize-1u)), n = _router[b];
		// no duplicates allowed, look for a match
		if(!_duplicate)
		{
			while(~n) 
				if (_data[n]._item == obj)
					return end();
				else
					n = _data[n]._next;
		}
		// if we made it this far, add the new item to the front
		n = _data.find_open();
		_data.set_valid( n, true );
		_data[n]._item = obj;
		_data[n]._next = _router[b]; // link the chain
		_router[b] = n; // adds the new node to the table
		// return an iterator to the new item
		return iterator(n,&_data);
	}

	// clear operation. flattens the routing table, to all OxFFFFFFFF
	// this ensures no left over chains
	// Also will call clear on the internal vector
	// this ensures free slots for new entries
	void clear()
	{
		// set the state of the routing table to unused entries
		memset(_router, 0xFFFFFFFF, _tablesize << 2);
		// dump all items stored by the internal vector
		_data.clear();
	}

	// clears and retains the routing table just like the "clear" function,
	// however it sets the capacity of the internal vector to zero, freeing up memory
	void _destroy()
	{
		// set the state of the routing table to unused entries
		memset(_router, 0xFFFFFFFF, _tablesize << 2);
		// free ram stored by the internal vector
		_data._destroy();
	}

	// returns the number of actual data entries in the table
	uInt32 size() const { return _data.size(); }
	// returns the number of routing buckets in the hash table
	uInt32 buckets() const { return _tablesize; }
	// reserves "n amount of data where n = (number of buckets)"
	void reserve() { _data.reserve(_tablesize); }
	// returns whether or not the ADT allows duplicates
	Bool8 duplicating() const { return _duplicates; }
	
};
// undefine constants and macros
#undef uInt32
#undef Int32
#undef Char8
#undef Bool8
#undef NEXT_PWR2
#undef IS_PWR2

#endif

// LEGACY CODE

// old remove function source
/*
uInt32 bucket = (_hash(obj) & (_tablesize-1u));
assert(bucket < _tablesize);
if(_router[bucket] == 0xFFFFFFFF) return 0;
assert(_router[bucket] < _data.size());
if(_data[_router[bucket]]._item == obj)
{
	uInt32 kill = _router[bucket]; 
	_router[bucket] = _data[_router[bucket]]._next;
	assert(_router[bucket] == 0xFFFFFFFF || _router[bucket] < _data.size()); 
	_data.set_valid( kill, false );
	_erase_count++;
	return 1u;
}
uInt32 prev = _router[bucket];
uInt32 curr = _data[prev]._next;
while(~curr)
{
	assert(curr < _data.size());
	if(_data[curr]._item == obj)
	{
		// bridge
		assert(prev < _data.size());
		_data[prev]._next = _data[curr]._next;
		assert(_data[prev]._next == 0xFFFFFFFF || _data[prev]._next < _data.size());
		// kill
		_data.set_valid( curr, false );
		_erase_count++;
		return 1u;
	}
	// move along
	prev = curr;
	curr = _data[curr]._next;
}
return 0u;
*/