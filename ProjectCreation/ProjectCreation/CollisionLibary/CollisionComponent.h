#pragma once
#include <DirectXMath.h>
#include <array>
#include <vector>
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
                ECollisionType            collisionType;
                std::vector<FCotactPoint> collideSurfaces;
        };

        struct FSphere
        {
                DirectX::XMVECTOR center;
                float             radius;
        };

        struct FAabb
        {
                DirectX::XMVECTOR center;  // min
                DirectX::XMVECTOR extents; // max
        };

        struct FPlane
        {
                DirectX::XMVECTOR normal;
                float             offset;
        };

        using Frustum = std::array<FPlane, 6>;

        struct FCapsule
        {
                DirectX::XMVECTOR startPoint;
                DirectX::XMVECTOR endPoint;
                float             radius;

				float GetCapsuleDistance();
        };
};