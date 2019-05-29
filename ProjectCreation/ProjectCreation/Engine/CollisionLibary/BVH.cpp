#include "BVH.h"
#include "../MathLibrary/MathLibrary.h"
#include "CollisionLibary.h"

using namespace Shapes;

void BVH::BVHTree::RecursiveCheckAABB(FAabb& aabb, BVHNode* node, std::vector<int>& tris)
{
        if (CollisionLibary::OverlapAabbToAabb(aabb, node->aabb(), 0.15f).collisionType == Collision::EOveralap ||
            CollisionLibary::OverlapAabbToAabb(aabb, node->aabb(), 0.15f).collisionType == Collision::ECollide)
        {
                if (node->is_branch())
                {
                        // debug_renderer::DrawAABB(node->aabb(), float4{1.0f, 0.f, 0.0f, 1.0f});
                        RecursiveCheckAABB(aabb, node->left(), tris);
                        RecursiveCheckAABB(aabb, node->right(), tris);
                }
                else
                {
                        tris.push_back(node->m_index);
                }
        }
}

void BVH::BVHTree::InsertAABB(FAabb& aabb, int index)
{
        if (root == nullptr)
        {
                root = new BVHNode(aabb, index);
        }

        else
        {
                BVHNode* curr = root;
                while (curr->is_branch())
                {
                        curr->aabb()   = CollisionLibary::AddAABB(aabb, curr->aabb());
                        float leftDis  = MathLibrary::ManhattanDistance(aabb, curr->left()->aabb());
                        float rightDis = MathLibrary::ManhattanDistance(aabb, curr->right()->aabb());
                        if (leftDis < rightDis)
                        {
                                curr = curr->left();
                        }
                        else
                        {
                                curr = curr->right();
                        }
                }
                BVHNode* left = new BVHNode(curr->aabb(), curr->m_index);
                left->parent(curr);
                curr->left(left);

                BVHNode* right = new BVHNode(aabb, index);
                right->parent(curr);
                curr->right(right);
                curr->aabb() = CollisionLibary::AddAABB(left->aabb(), right->aabb());
        }
}

std::vector<int> BVH::BVHTree::checkAABB(FAabb& aabb)
{
        std::vector<int> output;
        RecursiveCheckAABB(aabb, root, output);

        return output;
}

BVH::BVHNode::BVHNode(const Shapes::FAabb& bounds, int index) : _aabb{bounds}, m_index(index)
{}
