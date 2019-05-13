#pragma once
#include "../MathLibary/MathLibary.h"
#include<vector>
class CollisionComponent
{
    public:
        enum ECollisionType
        {
                ENoCollision,
                EOveralap,
                ECollide
        };

        struct FCotactPoint
        {
                DirectX::XMVECTOR position;
                DirectX::XMVECTOR normal;
        };
		struct FCollideResult
		{
                ECollisionType collisionType;
                std::vector<FCotactPoint> collideSurfaces;
		};
};