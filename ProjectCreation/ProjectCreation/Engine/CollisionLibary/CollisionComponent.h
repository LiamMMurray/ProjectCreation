#pragma once
#include <DirectXMath.h>
#include <vector>
#include "BVH.h"
#include "Shapes.h"

namespace Collision
{

        enum ECollisionType
        {
                ENoCollision,
                EOveralap,
                ECollide
        };

        struct FContactPoint
        {
                DirectX::XMVECTOR position;
                DirectX::XMVECTOR normal;
        };
        struct FCollideResult
        {
                ECollisionType             collisionType;
                std::vector<FContactPoint> collideSurfaces;
                DirectX::XMVECTOR          finalPosition;
        };

        struct FAdvancedviCollisionResult
        {
                ECollisionType             collisionType;
                std::vector<FContactPoint> collideSurfaces;
                DirectX::XMVECTOR          finalPosition;
                DirectX::XMVECTOR          finalDirection;
        };

        struct FCollisionObjects
        {
                std::vector<Shapes::FSphere> spheres;
                std::vector<Shapes::FAabb>   aabbs;
                std::vector<Shapes::FPlane>  planes;
        };

        enum ECollisionObjectTypes
        {
                Sphere = 0,
                Aabb, // = 1
                Plane, // = 2
                Capsule // = 3 
        };

        extern BVH::BVHTree bvhTree;
}; // namespace Collision