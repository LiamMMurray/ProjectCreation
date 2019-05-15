/************************************************
	Template Axis Aligned Bounding Cube Tree

	description :	A template BSP class that is coordinate system independent.
					Uses the ld_vector class to contain/recycle Node items.
					Uses ls_list to maintain a sorted list of items that fall underneath
					user defined volumes of space that need effecient spacial queries.
					The user defined volumes are fully dynamic and are optimized
					to ignore unessacary inquiries into the tree. (first 30 volumes only)

	Author : Lari H. Norri
	date started : 10/26/09
	last modified : 01/06/19 (in FINAL rev 1.4)

	Use :		Use it for anything that needs to be culled in 3D space. 
				Great for graphic applications and games that must 
				have thousands of static or dynamic	objects that need culling.

	Licence :	This code is free to use/modify whatever.
				Just mention me as the original author.

	Thanks :	The team at VMAX Technologies, Inc.

***********************************************/

#ifndef _T_AABC_TREE_H_ 
#define _T_AABC_TREE_H_

#include "ld_vector.h"
#include "ls_list.h"

// macro to determine absolute value
#define _ABS(v) (((v) > 0) ? (v) : -(v)) 
// bit mask for examinig item 2 volume relationships
#define _ITEM_MASK 0x3FFFFFFF	

// _val is what type you want the tree to hold, (must support < operator)
// _percision is the type you wish the bounding boxes to be comprised of. ex: float,int,double
template <typename _val, typename _percision>
class T_AABC_Tree
{
	// types of node splits
	enum { X_SPLIT, Y_SPLIT, Z_SPLIT };
	// defines the specific parts of a Bounding box.
	enum { LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK };
	// Define first volume bit, last volume bit, and dynamic link bit, finally the item kill bit
	enum { V0 = 1u, V29 = 536870912u, I_DYNAMIC = 1073741824u, I_KILL = 2147483648u };
	// This how a piece of data is tracked in the tree
	struct ITEM
	{
		// first 30bits reserved for volume identification, then dynamic bit, last bit is the kill switch.
		unsigned int flags;
		// how many volumes/lists are currently refrencing this data?
		unsigned int refCount; // used to delete an item when ref count falls to zero.
		// indexes, used to mark what node is looking at us, and our dynamic index if needed.
		unsigned int nIndex, iIndex, dIndex;
		// bounding cube for this item at time of insertion. (avoid bounding cube re-calculation)
		_percision cube[6]; // bounding cube
		_val data; // actual data
		ITEM() :	flags(0), refCount(0), nIndex(0xFFFFFFFF), 
					iIndex(0xFFFFFFFF), dIndex(0xFFFFFFFF) {} // default constructor
	};
	// A single node in the tree
	struct NODE
	{
		unsigned char depth; // how deep is this node?
		unsigned int visit; // record what volumes do not need to re-visit this data set.
		unsigned int positive;
		unsigned int negative;
		_percision BC[6]; // bounding cube
		ld_vector<unsigned int> items; // indexes to items being looked at				
		// initialize default values
		NODE() : depth(0), visit(0), positive(0xFFFFFFFF), negative(0xFFFFFFFF) 
		{ memset(BC,0,sizeof(_percision)); }
		// return the central plane of this node based on division plane
		_percision GetCenter(unsigned char divType)
		{
			_percision center = 0; // used to store the center of a node 
			if(divType == X_SPLIT) // Split on X axis 
				center = (_percision)(BC[LEFT] + (BC[RIGHT] - BC[LEFT]) * 0.5);
			else if(divType == Y_SPLIT)	// Split on Y axis
				center = (_percision)(BC[TOP] + (BC[BOTTOM] - BC[TOP]) * 0.5);
			else if(divType == Z_SPLIT)	// Split on Z axis
				center = (_percision)(BC[FRONT] + (BC[BACK] - BC[FRONT]) * 0.5);
			return center;
		}
		// returns true if a cube falls across the center of a node, 
		// "plane_bias" will be true if a cube leans toward a nodes positive plane, 
		// or false if a cube leans toward a nodes negative plane.
		bool AnalyzeCube(const _percision *cube, bool &plane_bias)
		{
			unsigned char divType = depth % 3; plane_bias = true;
			// compute center point of node based on divide type
			_percision center = GetCenter(divType);
			unsigned char low = 0, high = 1; // high and low planar values
			if(divType == Y_SPLIT) { low = 2; high = 3; }
			else if(divType == Z_SPLIT) { low = 4; high = 5; }
			if(cube[low] > cube[high]) // swap high and low, modify resulting plane bias
			{ unsigned char tmp = low; low = high; high = tmp; }
			// which split is best? // if true, positive is better
			plane_bias = (_ABS(center - cube[low]) > _ABS(center - cube[high])) ? !plane_bias : plane_bias;
			if(cube[low] < center && cube[high] > center) return true;
			return false; // does not fall across the node central plane
		}
		// Determine if this node must accept the incoming object based on size/depth
		bool AnalyzeDepth(const _percision *cube, unsigned int max_depth)
		{
			if(depth >= max_depth) return true; // we have reached max depth, force an add.
			unsigned char divType = depth % 3;
			if(divType == X_SPLIT && _ABS(cube[RIGHT] - cube[LEFT]) > (_ABS(BC[RIGHT] - BC[LEFT]) * 0.5)) 
				return true; // we must accept this item, it is too big for our children.
			else if(divType == Y_SPLIT && _ABS(cube[BOTTOM] - cube[TOP]) > (_ABS(BC[BOTTOM] - BC[TOP]) * 0.5)) 
				return true; // we must accept this item, it is too big for our children.
			else if(_ABS(cube[BACK] - cube[FRONT]) > (_ABS(BC[BACK] - BC[FRONT]) * 0.5)) 
				return true; // we must accept this item, it is too big for our children.			
			return false; // we can pass this node to our children if desired
		}
		// create a child node, argument specifies positive or negative child
		NODE CreateChild(bool child)
		{
			// if we already have a child, fail
			assert((child) ? (~positive) == 0 : (~negative) == 0); 
			// The new node to be added
			NODE add; add.depth = depth+1; 
			memcpy(add.BC,BC,sizeof(_percision) * 6);
			// calculate division type, (parent depth indicates the split of child nodes)
			unsigned char divType = depth % 3;
			// compute center point of parent based on divide type
			_percision center = GetCenter(divType);						
			// determine handedness of coordinate system and build new node BC
			if(divType == X_SPLIT)
				((BC[LEFT] < BC[RIGHT]) == child) ? add.BC[LEFT] = center : add.BC[RIGHT] = center;
			else if(divType == Y_SPLIT)
				((BC[TOP] < BC[BOTTOM]) == child) ? add.BC[TOP] = center : add.BC[BOTTOM] = center;
			else if(divType == Z_SPLIT)
				((BC[FRONT] < BC[BACK]) == child) ? add.BC[FRONT] = center : add.BC[BACK] = center;
			// return the new node
			return add;
		}
		// A node is only considered barren if it has no children and no elements of its own
		bool Barren() const { return (!items.size() && positive == 0xFFFFFFFF && negative == 0xFFFFFFFF); } 
	};
	// LINKS to data in the tree (ref counted)
	struct LINK
	{
		T_AABC_Tree *source;		
		unsigned int item_index;
		// is this link valid? operation assumes a safe link
		operator bool()const { return !(source->data[item_index].flags & I_KILL); }
		// How many refrences to this item exist?
		unsigned int RefCount()const { return source->data[item_index].refCount; }
		// mark this link for death (assumes valid link)
		void KillSwitchEngage() { source->data[item_index].flags |= I_KILL; }
		// report if this link is dynamic or not
		bool IsDynamic() { return source->data[item_index].flags & I_DYNAMIC; }
		// grab the items value (assumes safe link)
		_val& GetValue() { return source->data[item_index].data; } 
		const _val& GetValue() const { return source->data[item_index].data; } 
		// convert LINK to an item it is pointing at
		ITEM& GetItem() { return source->data[item_index]; } 
		const ITEM& GetItem() const { return source->data[item_index]; } 
		// overloaded < operator for sorting volumes
		bool operator<(const LINK& cmp)const
		{
			if(!(*this) || !cmp) return false;
			return	(source->SortCustom) 
					? source->SortCustom(GetValue(), cmp.GetValue()) 
					: GetValue() < cmp.GetValue();	
		}
		// link constructor takes an ITEM and increments refcount
		LINK(unsigned int i_index, T_AABC_Tree *tree) 
			: item_index(i_index), source(tree) 
		{
			assert(source); 
			assert(source->data.is_valid(i_index));
			source->AquireItem(i_index); // up ref count
		}
		// link copy constructor increments ref-count
		LINK(const LINK &cpy) 
		{ 
			source = cpy.source;			
			item_index = cpy.item_index;
			source->AquireItem(item_index); // up ref count	 
		}
		// link operator = incremnts ref count
		const LINK& operator=(const LINK& cpy) 
		{ 
			if(this != &cpy) 
			{ 
				source->ReleaseItem(item_index);
				item_index = cpy.item_index;
				source = cpy.source;
				source->AquireItem(item_index); // up ref count	
			} 
			return *this; 
		}
		// link destructor decrements ref count and removes item if possible
		~LINK() { source->ReleaseItem(item_index); }
	};

	// All items contained in this data structure
	ld_vector<ITEM> data;
	// Indexes to dynamic items in the tree
	ld_vector<unsigned int> dynamicData;
	// All nodes in this data structure
	ld_vector<NODE> tree; // front is always the root	
	// tree settings and options
	unsigned int minDepth, maxDepth, numItems;
	bool prune; // prune empty leaf nodes when found?

public:
	// allow the volume access to the tree LINKS
	class VOLUME; 
	friend class VOLUME;	
	// A container that holds a subset of data from the tree within a defined area.
	// Only 30 simultaneous volumes are item tracked at one time for optimiziation
	class VOLUME // though public, only T_AABC_Tree may create them
	{
		friend class T_AABC_Tree;
		// if no bits in the first 30 are toggled, it indicates an unoptimized volume
		unsigned int vID; // 0-30 id value, then 1 bit to signify if the volume has moved/changed size.
		_percision area[6];
		ls_list<LINK> contents;
		/// optional callbacks for data entering/leaving a volume.
		// Static per template Identifier for volumes 
		static unsigned int vCount;
	public:
		// Generated volumes are optimized to a particular 0-30 bit if possible.
		// volume constructor is mostly identical to the ls_list constructor.
		// except the external sort function MUST be provided to the tree class, not the volume.
		VOLUME(unsigned short sort_interval = 0, bool recycle_links = false) 
				: contents(sort_interval,0,recycle_links), vID(0)
		{ 
			memset(area,0,sizeof(_percision)*6);
			// look for a free bit in 30, use one if we find one for optimization.
			unsigned int free = 1u;
			if(VOLUME::vCount != _ITEM_MASK)
			{   // find a free bit in the first 30 
				while(free & VOLUME::vCount) free = free << 1;
				VOLUME::vCount |= free; vID |= free; 
			}
		}
		// destructor frees all links and use bit
		~VOLUME() 
		{   
			Clear(); // notify all items we no longer contain them
			// free the optimiztion bit if we have one
			if(vID & _ITEM_MASK) VOLUME::vCount &= ~(vID & _ITEM_MASK);			
		} 
		// set the dimensions of the volume (view cube) // the cube will only be modified if a dimension exceeds filter
		void SetVolumeCube(_percision L, _percision R, _percision T, _percision B, _percision N, _percision F, _percision Filter = 0)
		{
			_percision cube[6] = {L,R,T,B,N,F}; // form cube
			//if(memcmp(area,cube,sizeof(_percision)*6)) // if the cube has changed, toggle change flag
			if(	_ABS(area[0]-L) > Filter || _ABS(area[1]-R) > Filter || _ABS(area[2]-T) > Filter ||
				_ABS(area[3]-B) > Filter || _ABS(area[4]-N) > Filter || _ABS(area[5]-F) > Filter)
			{
				memcpy(area,cube,sizeof(_percision)*6);
				vID |= I_DYNAMIC; // volume modified
			}
		}
		// set the callback operations when a new object enters/leaves this volume. (provide NULL for no action)
		
		// clear a volume of all linked items. (required by unoptimized volumes)
		void Clear() 
		{ 
			// notify all valid items, they no longer belong (v1.4 added c++11 dependent name conformance)
			typename ls_list<LINK>::iterator x = contents.begin(); // link iterator
			for(;x != contents.end(); ++x)
				if(*x) x->GetItem().flags &= ~(vID & _ITEM_MASK);
			contents.clear(); // remove all links
		}
		// during query, remove any invalid/out-of-bound links, call callbacks if specified.
		void UpdateVolume( T_AABC_Tree &AABCTree ) 
		{
			// if a volume is considered "unoptimized" ie: not one of the first 30 created via the tree. 
			if(!(vID << 2)) Clear(); // it must be cleared since tree items have no memory of this volume.
			// go through links, remove dead or out of bound items.
			// (v1.4 added c++11 dependent name conformance e.g: "typename")
			typename ls_list<LINK>::iterator x = contents.begin(); // link iterator
			while(x != contents.end())
			{
				if(!(*x)) x = contents.erase(x); 
				else // if an item is out of bounds, remove it's volume bit. 
				{
					///AABCTree.GetBoundingCube(BC,x->GetValue()); // get cube
					if(!AABCTree.BCCollision(area,x->GetItem().cube)) // test cube
					{// flag this item as no longer being in this volume
						x->GetItem().flags &= ~(vID & _ITEM_MASK);
						x = contents.erase(x); // remove link from volume
					} 
					else ++x; // check next item
				} 				 
			}
			// Fill volume with any links found from the tree.
			AABCTree.FillVolume(*this);
			vID &= ~I_DYNAMIC; // mark this volume as clean. 
		}
		// Forces the contents of the volume to be sorted instanly. (slow)
		void SortVolume() { contents.force_sort(); }
		// iteration should simply skip over invalid items, and access valid items as "_val"
		class iterator
		{
			friend class VOLUME;
			// internal list iterator
			typedef typename ls_list<LINK>::iterator LinkIter; // avoid C4346
			LinkIter i; // internal ls_list iterator
		public:
			iterator(LinkIter x) : i(x) { }
			_val *operator->() const { return &i->GetValue(); }
			_val &operator*() const { return i->GetValue(); }
			operator bool() const { return i; } 				
			iterator &operator++() 
			{ 
				// if the next item is invalid, skip it
				while(++i && !(*i)) {} 
				return (*this); 
			}				
			iterator &operator--() 
			{ 					
				// if the next item is invalid, skip it
				while(--i && !(*i)) {} 				
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
		// erase function takes a volume iterator and flags it for eventual tree removal
		iterator erase(iterator x) { x.i->KillSwitchEngage(); return ++x; }
		// begin, the first iterator
		iterator begin() { return iterator(contents.begin()); }
		// end, the last iterator
		iterator end() { return iterator(contents.end()); }
		// check an iterator to see if the item is dynamic
		bool IsDynamic(iterator chk) { return (chk) ? chk.i->IsDynamic() : false; }		
	};
private:

	// function pointer used to convert a "_val" to a bounding cube
	void (*GetBoundingCube)(_percision*, const _val &);
	// function pointer to custom sort function used to sort "_val" within the tree. (used by volumes)
	bool(*SortCustom)(const _val&, const _val&); // same format as used by ls_list!
	// this bounding cube function detects collisions, regardless of the polarity of your coordinate system 
	bool BCCollision(const _percision *BC_1, const _percision *BC_2) const;
	// remove an item from the tree, it may not be removed right away if volumes are refrencing it.
	///void RemoveItem(unsigned int nIndex, unsigned int nIndex);
	// Recursive functions, called by public versions	
	// Insert an item into the tree
	void Insert(unsigned int nIndex, const _percision *cube, const _val &item, bool dynamic);
	// Re-Insert An Item back into the tree under a new node
	void ReInsertItem(unsigned int nIndex, unsigned int item, const _percision *cube); 
	// Fill a volume with links
	void FillVolume(unsigned int nIndex, VOLUME &fill);
	// Aquire an item for refrence purposes (up the ref count!)
	void AquireItem(unsigned int item_index);
	// Release an item for refrence purposes (down the ref count, and possily delete)
	void ReleaseItem(unsigned int item_index);
public:
	// constructor, takes in the dimmensions of the entire tree.
	// Bounding Boxes are always passed as an array of 6, in the order LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK
	// The class must be initialized with a function pointer that can extract bounding data from type "_val"
	// optional arguments: "Sort" custom sort routine used by volumes to determine the order of items when iterating.
	// "max_depth" the maximum node depth allowed in the tree. "min_depth" an item must be inserted at least this deep.
	// "min_depth" is only enforced if an item can potentially fit within a child node yet falls on the parent node boundry.
	// This special case avoids producing excessive culling artifacts with "min_depth" set to a high value.
	// pruning determines if empty leaf nodes are erased when no longer needed.
	T_AABC_Tree(const _percision *world_BC, void(*GetBC)(_percision*,const _val&), bool(*Sort)(const _val&,const _val&) = 0,
				unsigned int max_depth = 12, unsigned int min_depth = 0, bool pruning = true) 
				: maxDepth(max_depth), minDepth(min_depth), numItems(0), GetBoundingCube(GetBC), SortCustom(Sort)
	{
		assert(GetBoundingCube); // We MUST have a cube extractor function
		// create the root node
		NODE root; memcpy(root.BC,world_BC,sizeof(_percision) * 6);
		tree.push_back(root); // add root to tree
		// pruning is on by default (saves a lot of memory when dynamic objects are on)
		prune = pruning;
	}
	// Insert an item into the tree
	void Insert(const _val &item, bool dynamic = false) 
	{ 
		_percision cube[6]; // cube memory
		GetBoundingCube(cube,item); // get cube
		Insert(0,cube,item,dynamic); // add item
	}
	// Fill a volume with any items that fall underneath it. Mark items linked as visited.
	void FillVolume(VOLUME &fill) 
	{	
		if(!tree.size()) return;
		// add any static items. (+discover unrefrenced dynamics)
		FillVolume(0,fill);
	}
	// removes then re-inserts all dynamic items in the tree. (don't call this TOO often)
	void UpdateDynamicItems();
	// Clear the contents of the tree but perserve the root node.
	void Clear() 
	{ 
		if(!tree.size()) return;
		NODE root; memcpy(root.BC,tree.front().BC,sizeof(_percision)*6); 
		tree.clear(); data.clear(); dynamicData.clear(); 
		tree.push_back(root); 
	}
}; // end tree class definition

// begin function definitions

// Aquire an ITEM from the tree to link to
template <typename _val, typename _percision>
void T_AABC_Tree<_val, _percision>::AquireItem(unsigned int item_index)
{
	assert(data.is_valid(item_index));
	++data[item_index].refCount;
}
// free an ITEM from being refrenced
template <typename _val, typename _percision>
void T_AABC_Tree<_val, _percision>::ReleaseItem(unsigned int item_index)
{
	assert(data.is_valid(item_index));
	assert(data[item_index].refCount > 0);	
	if(!(--data[item_index].refCount) && (data[item_index].flags & I_KILL))
	{ // free this item completely if it is no longer refrenced
		if(~data[item_index].nIndex) // node ref removed
			tree[data[item_index].nIndex].items.set_valid(data[item_index].iIndex,false); 	
		if(~data[item_index].dIndex) // dynamic ref removed
			dynamicData.set_valid(data[item_index].dIndex,false);
		data.set_valid(item_index,false); // delete item
	}
}

// Insert an item into the tree
template <typename _val, typename _percision>
void T_AABC_Tree<_val, _percision>::Insert(unsigned int nIndex, const _percision *cube, const _val &item, bool dynamic)
{
	// determine if this item belongs in this node.
	bool bias; // what side does it lean? 
	// belongs if max depth is reached or falls on border
	// does not belong if min depth is not yet reached or does not fall on border.
	if((tree[nIndex].AnalyzeCube(cube,bias) && tree[nIndex].depth >= minDepth) || tree[nIndex].AnalyzeDepth(cube,maxDepth))
	{
		ITEM add; add.data = item; add.nIndex = nIndex; // add initial values
		unsigned int iIndex = data.push_back(add);		
		if(dynamic) // add a index used for periodic re-insertion
		{ data[iIndex].dIndex = dynamicData.push_back(iIndex); data[iIndex].flags |= I_DYNAMIC; } 		
		data[iIndex].iIndex = tree[nIndex].items.push_back(iIndex);
		// copy over the bounding cube for this item so we don't have to re-calculate it later.
		memcpy(data[iIndex].cube,cube,sizeof(_percision)*6);
		tree[nIndex].visit = 0u; // notify all volumes a re-visit is needed		
		return; // all done here
	} 		
	// generate child node based on bias if we do not already have one.
	unsigned int child = (bias) ? ((~tree[nIndex].positive) ? tree[nIndex].positive : tree.push_back(tree[nIndex].CreateChild(bias)))
								: ((~tree[nIndex].negative) ? tree[nIndex].negative : tree.push_back(tree[nIndex].CreateChild(bias)));	
	// recurse on child node. Ensure node link is maintained
	(bias) ? tree[nIndex].positive = child : tree[nIndex].negative = child;
	Insert(child,cube,item,dynamic); // place item further in tree
}

// Re-Insert an item into the tree
template <typename _val, typename _percision>
void T_AABC_Tree<_val, _percision>::ReInsertItem(unsigned int nIndex, unsigned int item, const _percision *cube)
{
	// determine if this item belongs in this node.
	bool bias; // what side does it lean? 
	// belongs if max depth is reached or falls on border
	// does not belong if min depth is not yet reached or does not fall on border.
	if((tree[nIndex].AnalyzeCube(cube,bias) && tree[nIndex].depth >= minDepth) || tree[nIndex].AnalyzeDepth(cube,maxDepth))
	{
		// Notify volumes a re-visit of this node is required. 
		// Even if the item's node has not changed, the item may have moved into view of the volume.
		tree[nIndex].visit = 0u; 
		data[item].nIndex = nIndex; // add initial values
		data[item].iIndex = tree[nIndex].items.push_back(item); 
		return; // all done here
	} 		
	// generate child node based on bias if we do not already have one.
	unsigned int child = (bias) ? ((~tree[nIndex].positive) ? tree[nIndex].positive : tree.push_back(tree[nIndex].CreateChild(bias)))
								: ((~tree[nIndex].negative) ? tree[nIndex].negative : tree.push_back(tree[nIndex].CreateChild(bias)));	
	// recurse on child node. Ensure node link is maintained
	(bias) ? tree[nIndex].positive = child : tree[nIndex].negative = child;
	ReInsertItem(child,item,cube); // place item further in tree
}

// removes then re-inserts all dynamic items in the tree. (don't call this TOO often)
template <typename _val, typename _percision>
void T_AABC_Tree<_val, _percision>::UpdateDynamicItems()
{
	///_percision BC[6];
	// loop through list of dynamics.	
	ld_vector<unsigned int>::iterator x = dynamicData.begin();
	for(;x != dynamicData.end(); ++x) 
	{
		// remove item from current location
		tree[data[*x].nIndex].items.set_valid(data[*x].iIndex,false);
		// add item back to tree node
		GetBoundingCube(data[*x].cube,data[*x].data); // get cube
		ReInsertItem(0,*x,data[*x].cube); // place back in tree
	}
}

// Fill a volume with links
template <typename _val, typename _percision>
void T_AABC_Tree<_val, _percision>::FillVolume(unsigned int nIndex, VOLUME &fill)
{		
	///_percision BC[6];// bounding cube to be filled by data
	// first thing first, does this node collide with our VOLUME?
	if(BCCollision(tree[nIndex].BC,fill.area))
	{
		// check the bits, have we already visited this node?
		// if yes and volume is marked as unchanged, continue on to children
		if(!((tree[nIndex].visit & fill.vID) << 2) || fill.vID & I_DYNAMIC)
		{			
			// if no check each item if collides with with the volume and if not already linked to it, add it. (check bits)
			ld_vector<unsigned int>::iterator x = tree[nIndex].items.begin();
			for(;x != tree[nIndex].items.end(); ++x)
			{
				if(!((data[*x].flags & fill.vID) << 2))
				{	
					///GetBoundingCube(BC,data[*x].data); // get cube
					if(BCCollision(fill.area,data[*x].cube))
					{
						// create a link and add to our link set
						data[*x].flags |= (fill.vID & _ITEM_MASK);
						LINK track(*x,this);
						fill.contents.push_front(track); // push front, is better for sorting
					}
				}
			}
			// once the node has been scanned, mark the node as visited by this volume
			tree[nIndex].visit |= (fill.vID & _ITEM_MASK);
		}
		// move on to children nodes
		if(~tree[nIndex].positive) FillVolume(tree[nIndex].positive,fill);
		if(~tree[nIndex].negative) FillVolume(tree[nIndex].negative,fill);
	}
	// last step... if the node is empty, has no children and pruning is on. (and is not root) delete this node.
	if(	prune )
	{
		if(~tree[nIndex].positive && tree[tree[nIndex].positive].Barren())
		{ // break the link and remove the node	
			tree.set_valid(tree[nIndex].positive,false);
			tree[nIndex].positive = 0xFFFFFFFF;
		}
		if(~tree[nIndex].negative && tree[tree[nIndex].negative].Barren())
		{ // break the link and remove the node	
			tree.set_valid(tree[nIndex].negative,false);
			tree[nIndex].negative = 0xFFFFFFFF;
		}
	}
}

// this bounding cube function detects collisions, regardless of the polarity of your coordinate system 
template <typename _val, typename _percision>
bool T_AABC_Tree<_val, _percision>::BCCollision(const _percision *BC_1, const _percision *BC_2) const
{
	// x coordinate system is right handed 
	if(BC_1[LEFT] < BC_1[RIGHT])
	{	
		if(BC_1[LEFT] > BC_2[RIGHT]) return false;
		if(BC_1[RIGHT] < BC_2[LEFT]) return false;
	}
	else
	{
		if(BC_1[LEFT] < BC_2[RIGHT]) return false;
		if(BC_1[RIGHT] > BC_2[LEFT]) return false;
	}
	// y coordinate system is up positive 
	if(BC_1[TOP] > BC_1[BOTTOM])
	{	
		if(BC_1[TOP] < BC_2[BOTTOM]) return false;
		if(BC_1[BOTTOM] > BC_2[TOP]) return false;
	}
	else
	{
		if(BC_1[TOP] > BC_2[BOTTOM]) return false;
		if(BC_1[BOTTOM] < BC_2[TOP]) return false;
	}
	// z coordinate system is near positive 
	if(BC_1[FRONT] > BC_1[BACK])
	{	
		if(BC_1[FRONT] < BC_2[BACK]) return false;
		if(BC_1[BACK] > BC_2[FRONT]) return false;
	}
	else
	{
		if(BC_1[FRONT] > BC_2[BACK]) return false;
		if(BC_1[BACK] < BC_2[FRONT]) return false;
	}
	return true;	
}

// enable the volume to track bits
template <typename _val, typename _percision>
unsigned int T_AABC_Tree<_val, _percision>::VOLUME::vCount(0); 

// no longer needed
#undef _ABS
#undef _ITEM_MASK
#endif
