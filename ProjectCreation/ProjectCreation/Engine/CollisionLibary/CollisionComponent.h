#pragma once
#include <DirectXMath.h>
#include <vector>
#include"Shapes.h"
namespace CollisionComponent
{

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

        struct FSweepCollisionResult
        {
                ECollisionType            collisionType;
                std::vector<FCotactPoint> collideSurfaces;
                DirectX::XMVECTOR         finalPosition;
        };
        
        struct FCollisionObjects
        {
                std::vector<Shapes::FSphere> spheres;
                std::vector<Shapes::FAabb>     aabbs;
                std::vector<Shapes::FPlane>    planes;
                
        };

};