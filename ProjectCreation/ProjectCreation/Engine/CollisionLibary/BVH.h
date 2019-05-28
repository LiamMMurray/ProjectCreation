#pragma once
#include <vector>
#include "Shapes.h"

namespace BVH // Bounding Volume Hierarchies
{
        class BVHNode
        {
            public:
                // TODO: This constructor is the only function for you to implement in this file.
                // bvh_node_t(bvh_node_t* root, uint32_t left_index, uint32_t right_index);

                BVHNode(const Shapes::FAabb& bounds, int index);

                BVHNode()               = default;
                BVHNode(const BVHNode&) = default;

                // Root must be at [0], so parent index must be invalid
                inline bool is_root() const
                {
                        return _parent == nullptr;
                }

                inline bool is_branch() const
                {
                        return _left != nullptr;
                }

                inline bool is_leaf() const
                {
                        return !is_branch();
                }


                inline void left(BVHNode* ptr)
                {
                        _left = ptr;
                }

                inline BVHNode* left() const
                {
                        return _left;
                }

                inline void right(BVHNode* ptr)
                {
                        _right = ptr;
                }

                inline BVHNode* right() const
                {

                        return _right;
                }

                inline BVHNode* parent() const
                {

                        return _parent;
                }

                inline void parent(BVHNode* ptr)
                {
                        _parent = ptr;
                }

                inline Shapes::FAabb& aabb()
                {
                        return _aabb;
                }

                inline const Shapes::FAabb& aabb() const
                {
                        return _aabb;
                }

                int m_index;

            private:
                // Indices for connected BVHNode
                BVHNode* _parent;

                // Index for left child.
                // Branches must have two children.
                // If "this" is a leaf, _left/_is_branch will be 0.

                BVHNode* _left = nullptr;


                // Index for right child.
                // Branches must have two children.
                // If "this" is a leaf, _right/_id stores the element id.

                BVHNode* _right = nullptr;
                // AABB for this node
                Shapes::FAabb _aabb;
        };

        class BVHTree // bvh tree is for static object; give every objects an aabb as a bounding box to add in to the bvh tree
        {
                void RecursiveCheckAABB(Shapes::FAabb& aabb, BVHNode* node, std::vector<int>& tris);

            public:
                BVHNode*         root = nullptr;
                void             InsertAABB(Shapes::FAabb& aabb, int index);
                std::vector<int> checkAABB(Shapes::FAabb& aabb);
        };
} // namespace BVH