#include "BVH.h"
#include"CollisionLibary.h"
#include"../MathLibrary/MathLibrary.h"
using namespace Shapes;
void BVH::BVHTree::RecursiveCheckAABB(FAabb& aabb, BVHNode* node, std::vector<int>& tris)
{
        if (CollisionLibary::AabbToAabb(aabb, node->aabb(), 0.15f).collisionType == CollisionComponent::EOveralap ||
            CollisionLibary::AabbToAabb(aabb, node->aabb(), 0.15f).collisionType == CollisionComponent::ECollide)
        {
                if (node->is_branch())
                {
                        //debug_renderer::DrawAABB(node->aabb(), float4{1.0f, 0.f, 0.0f, 1.0f});
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
                        curr->aabb()   = CollisionLibary::AdddAABB(aabb, curr->aabb());
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
                curr->aabb() = CollisionLibary::AdddAABB(left->aabb(), right->aabb());
        }
}

std::vector<int> BVH::BVHTree::checkAABB(FAabb& aabb)
{
        std::vector<int> output;
        RecursiveCheckAABB(aabb, root, output);

        return output;
}
