#pragma once
#include <DirectXMath.h>
#include <vector>
#include "BVH.h"
#include "Shapes.h"
#include "../../ECS/ECSTypes.h"

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
               // DirectX::XMVECTOR          finalPosition;
        };

        struct FAdvancedCollisionResult
        {
                ECollisionType             collisionType;
                std::vector<FContactPoint> collideSurfaces;
                DirectX::XMVECTOR          finalPosition;
                DirectX::XMVECTOR          finalDirection;
        };

		struct FCollisionQueryResult
        {
                std::vector<ComponentHandle> spheres;
                std::vector<ComponentHandle> AABBs;
                std::vector<ComponentHandle> capsules;
        };

}; // namespace Collision