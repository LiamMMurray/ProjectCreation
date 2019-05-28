#pragma once
#include <DirectXMath.h>
#include <vector>
#include "Shapes.h"
#include"BVH.h"

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
                ECollisionType            collisionType;
                std::vector<FContactPoint> collideSurfaces;
                DirectX::XMVECTOR         finalPosition;
        };

        struct FSweepCollisionResult
        {
                ECollisionType            collisionType;
                std::vector<FContactPoint> collideSurfaces;
                DirectX::XMVECTOR         finalPosition;
                DirectX::XMVECTOR         finalDirection;
        };

        struct FCollisionObjects
        {
                std::vector<Shapes::FSphere> spheres;
                std::vector<Shapes::FAabb>   aabbs;
                std::vector<Shapes::FPlane>  planes;
        };

        struct FCollisionObjectTypes
        {
                Shapes::FSphere  spheres;
                Shapes::FAabb    aabbs;
                Shapes::FPlane   planes;
                Shapes::FCapsule capsule;
        };

		extern BVH::BVHTree bvhTree;
}; // namespace Collision