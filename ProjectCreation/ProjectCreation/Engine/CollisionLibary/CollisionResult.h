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

        struct FAdvancedCollisionResult
        {
                ECollisionType             collisionType;
                std::vector<FContactPoint> collideSurfaces;
                DirectX::XMVECTOR          finalPosition;
                DirectX::XMVECTOR          finalDirection;
        };

		struct FCollisionObjects
		{
                std::vector<Shapes::FAabb> aabbs;
                std::vector<Shapes::FSphere> spheres;
                std::vector<Shapes::FCapsule> capsules;
		};

}; // namespace Collision