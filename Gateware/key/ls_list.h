/************************************************
	Lazy Sort List

	description :
	
	the lazy sort list is a standard doubly 
	linked list	except for the fact that it 
	sorts itself over time as you iterate 
	through it. You must provide the "< "
	operator if you wish to use this with
	a custom class.

	Author : Lari H. Norri
	date started : 02/11/04
	last modified : 01/15/10 (Final rev. 2.0)

	Use :	Use it for anything that you wish to be
			sorted over time.  Mimics the STL list
			in use. but only has standard functionality
	
	Licence :	This code is free to use/modify whatever.
				Just mention me as the original author.
	
	Thanks :	Hewlet Packard for your amazing STL,
				and Tony Whitaker for his input. 
				Thx 2 Jeff Jackowski for teaching 
				me about in-place new/delete (now fixed).

***********************************************/

#ifndef _LS_LIST_H_
#define _LS_LIST_H_

#include <memory.h>

// ls_list now takes 64bit addressing into account
#ifdef _M_X64
	// defines a variable consisting of at least 128 bits. (2 64bit pointers)
	struct _INT_128_{unsigned __int64 _128[2];}; //ugly, but it works
	#define _address_x_2_ _INT_128_
#else 
	// defines a variable consisting of at least 64 bits. (2 32bit pointers)
	#define _address_x_2_ unsigned __int64
#endif

template <typename T>
class ls_list
{
	private :

		// size of the container
		unsigned int _size;

		// standard doubly linked list node
		struct Node	
		{
			Node *next;
			Node *prev;
			T data;
			// use copy constructor on node data so no default constructor is needed
			Node(const T &d) : data(d) {}
		};

		// a "dataless" node, must be 64 or 128 bits.
		// depending on your processor.
		// this "node's" memory address is used to 
		// represent one past the end of a list,
		// and to represent the node preceding 
		// the very first node.
		_address_x_2_ zero_node; // fakes 2 pointers

		// describes the behavior of a list
		// as well as its starting/end point
		struct Arbitrator
		{
			// Node* friendly represntation of zero_node
			Node *_zero;		
			// s_interval defines how many iterations there are per sort (0 for no sorting),
			// chain tallys how many iterations have happened, so it can determine when to sort next
			unsigned short s_interval, chain;
			// user defined function for sorting, (null if unused)
			// if null the list uses "T's" < operator.
			bool (*sort)(const T &a, const T &b);
		};

		Arbitrator arbiter;

		// If you enable the tree to recycle nodes, they are kept here
		Node *free_list; bool _recycle;

		// allocates a new node if required (uses copy constructor)
		Node *_alloc(const T& d) 
		{
			if(!_recycle || !free_list) 
			{ return new (malloc(sizeof(Node))) Node(d); }
			Node *used = free_list; 
			free_list = free_list->next;
			new (&used->data) T(d); // copy construct in-place
			return used; // return re-used memory
		}
		// deletes a node or recycles it
		void _free(Node *kill)
		{
			if(!_recycle) 
			{ kill->~Node(); free(kill); return; }
			kill->data.~T(); // delete in-place
			kill->next = free_list;
			free_list = kill;
		}

	public :

		// destroy all recyclable nodes from the list
		void destroy()
		{
			Node *kill;
			while(free_list)
			{
				kill = free_list->next;
				free(free_list);
				free_list = kill;
			}
		}

		class iterator;
		friend class iterator;

		// lets define the nested iterator class here
		class iterator
		{
			friend class ls_list;
			
			Node *current;
			Arbitrator *envoy;
			
			// swap two nodes
			void swap(Node* n1, Node* n2) 
			{ 
				n2->next->prev = n1;
				n1->prev->next = n2;
				n1->next = n2->next; n2->next = n1;
				n2->prev = n1->prev; n1->prev = n2; 
			}

			public :
				iterator(Node *n = 0, Arbitrator *a = 0) : current(n), envoy(a)	{ }

				T *operator->() const { return &current->data; }
				T &operator*() const { return current->data; }

				bool operator==(const iterator &comp) const 
				{	return current == comp.current;	}
				
				bool operator!=(const iterator &comp) const 
				{	return current != comp.current;	}

				// is false if the iterator is not at a valid location
				operator bool() const { return current != envoy->_zero; } 
				
				iterator &operator++() 
				{
					Node *cn,  *cnn;  // current->next and current->next->next
					// sort next items if possible
					if ((envoy->s_interval &&
					++envoy->chain >= envoy->s_interval) &&
					((cn = current->next) != envoy->_zero &&
					(cnn = cn->next) != envoy->_zero) &&
					// use the sort function if possible, otherwise use '<' operator
					((envoy->sort) ? envoy->sort(cn->data, cnn->data)
					: cn->data < cnn->data))
					{	
						swap(cn, cnn); // switch the nodes	
						envoy->chain = 0; // wait for next sort
					}
					// move to the next item; cn might not be set and may have been swapped
					current = current->next; 
					return (*this); 
				}
				
				iterator &operator--() 
				{ 
					Node *cp,  *cpp;  // current->prev and current->prev->prev
					// sort prev items if possible
					if ((envoy->s_interval && ++envoy->chain >= envoy->s_interval) &&
					((cp = current->prev) != envoy->_zero &&
					(cpp = cp->prev) != envoy->_zero) &&
					// use the sort function if possible, otherwise use '<' operator
					((envoy->sort) ? envoy->sort(cpp->data, cp->data)
					: cpp->data < cp->data))
					{	
						swap(cpp, cp);  // switch the nodes
						envoy->chain = 0; // wait for next sort
					}
					// move to the previous item; cp might not be set and may have been swapped
					current = current->prev; 
					return (*this); 
				}

				iterator operator++(int)
				{
					iterator tmp = *this;
					++*this;
					return tmp;
				}

				iterator operator--(int)
				{
					iterator tmp = *this;
					--*this;
					return tmp;
				}
		};
		// end of iterator class definition

		// define the ls_list constructor
		// first parameter is the frequency of sorting... 0 for no sort
		// second parameter is a custom sort operation... 0 for "<" operator
		// third parameter is whether the list recycles nodes. (memory vs speed)
		ls_list(unsigned short sort_interval = 0, 
				bool(*sort_func)(const T &a, const T &b) = 0,
				bool recycle_nodes = false)
		{
			_size = 0;
			arbiter.s_interval = sort_interval;
			arbiter.sort = sort_func;
			free_list = 0;
			_recycle = recycle_nodes;
			// define defaults
			arbiter._zero = (Node*)&zero_node;
			arbiter._zero->next = arbiter._zero->prev = arbiter._zero;
			arbiter.chain = 0;
		}
		
		// destructor + trilogy of evil
		~ls_list() { clear(); destroy(); }
		// copy constructor
		ls_list(const ls_list<T> &cpy)
		{
			// set up zero node (very important)
			arbiter._zero = (Node*)&zero_node;
			arbiter._zero->next = arbiter._zero->prev = arbiter._zero;
			free_list = 0; // no nodes pre-allocated
			*this = cpy; // copy all data
		}
		// assigment operator=
		ls_list<T> &operator=(const ls_list<T> &cpy)
		{
			if(&cpy == this) return *this; // safety
			// out with the old
			clear();
			// in with the new
			Node *transfer = cpy.arbiter._zero->next;
			while(transfer != cpy.arbiter._zero)
			{
				push_back(transfer->data);
				transfer = transfer->next;
			}
			// transfer vars
			_size = cpy._size;
			arbiter.chain = cpy.arbiter.chain;
			arbiter.s_interval = cpy.arbiter.s_interval;
			arbiter.sort = cpy.arbiter.sort;
			_recycle = cpy._recycle;
			return *this;// were done
		}

		// return the size of the container
		unsigned int size() const { return _size; } 

		// return an iterator to the front of the list
		iterator begin() { return ++iterator(arbiter._zero, &arbiter); }
		// return an iterator to one past the end of the list
		iterator end() { return iterator(arbiter._zero, &arbiter); }

		// adds an item to the front of the list
		void push_front(const T &data)
		{
			Node *n = _alloc(data);
			///n->data = data;
			n->next = n->prev = arbiter._zero;
			if (arbiter._zero->next != arbiter._zero)
			{	
				n->next = arbiter._zero->next;
				n->prev = arbiter._zero;
				arbiter._zero->next->prev = n; 
				arbiter._zero->next = n;
			}
			else
			{	
				arbiter._zero->next = n; 
				arbiter._zero->prev = n;	
			}
			++_size;
		}

		// adds an item to the end of the list
		void push_back(const T &data)
		{
			Node *n = _alloc(data);
			///n->data = data;
			n->next = n->prev = arbiter._zero;
			if (arbiter._zero->prev != arbiter._zero)
			{
				n->prev = arbiter._zero->prev;
				n->next = arbiter._zero;
				arbiter._zero->prev->next = n; 
				arbiter._zero->prev = n;
			}
			else
			{	
				arbiter._zero->next = n; 
				arbiter._zero->prev = n;	
			}
			++_size;
		}

		// retreive the topmost item
		T &front() { return arbiter._zero->next->data;	}
		const T &front() const { return arbiter._zero->next->data;	}
		// retreive the last item
		T &back() {	return arbiter._zero->prev->data;	}
		const T &back() const {	return arbiter._zero->prev->data;	}

		// remove an item from the linklist 
		iterator erase(iterator remove)
		{
			Node *rmv = remove.current;
			rmv->prev->next = rmv->next;
			rmv->next->prev = rmv->prev;
			remove = iterator(rmv->next, &arbiter);	// id like a better way to do this
			_free(rmv);
			--_size;
			return remove;
		}

		// remove an element from the front of the list
		void pop_front(void) { if(_size) erase(begin()); }
		// remove an element from the back of the list
		void pop_back(void) { if(_size) erase(--end()); }

		// tells us if the list is empty or not
		bool empty(void) const { return _size == 0; }

		// Insert an element before a specific point
		// returns an iterator to the newly inserted element
		iterator insert(iterator point, const T &data)
		{
			Node *ins = point.current;
			ins->prev->next = _alloc(data);
			///ins->prev->next->data = data;
			ins->prev->next->prev = ins->prev;
			ins->prev->next->next = ins;
			ins->prev = ins->prev->next;
			++_size;
			return iterator(ins->prev, &arbiter);
		}

		// Uses insertion sort to force all data to be sorted perfectly
		// Only call this function if your data MUST be sorted RIGHT NOW!!!
		void force_sort() // actually this is VERY SLOW. 
		{
			// Dont use this unless you HAVE to!
			// I'll convert this to merge sort one day
			if (_size < 2u) // no need to sort
				return;
			// terrible algorithim
			Arbitrator save = arbiter; // save current state
			// set the arbiter for sorting
			arbiter.chain = 1u; arbiter.s_interval = 1u;
			bool run = true; // do we continue forward?
			iterator sort = ++begin();
			while(sort != end())
			{
				if(run)
				{
					run = (arbiter.chain) ? true : false;
					++sort;
				}
				else
				{
					--sort;
					run = (arbiter.chain) ? true : false;
				}
			}
			arbiter = save; // back to normal

			// This ALSO WORKS BUT IS JUST AS SLOW
			//Arbitrator save = arbiter; // save current state
			//// set the arbiter for sorting
			//arbiter.chain = 1u; arbiter.s_interval = 1u;
			//iterator sort = ++begin(); // start sorting
			//while(sort != end()) // back track till no sort triggers 
			//	(!arbiter.chain) ? ++sort : --(--sort);
			//arbiter = save; // back to normal
		}

		// remove all data from the list
		void clear(void)
		{
			if (arbiter._zero->next == arbiter._zero)
				return;				
			
			while(arbiter._zero->next->next != arbiter._zero)
			{
				arbiter._zero->next = arbiter._zero->next->next;
				_free(arbiter._zero->next->prev);
			}
			_free(arbiter._zero->next);
			
			// reset the list
			arbiter._zero->next = arbiter._zero->prev = arbiter._zero;
			_size = 0;
		}
};

#undef _address_x_2_

#endif