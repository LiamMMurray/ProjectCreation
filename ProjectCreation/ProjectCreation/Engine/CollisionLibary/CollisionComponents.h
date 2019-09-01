#pragma once
#include <unordered_set>
#include <Component.h>
#include "CollisionResult.h"
#include "Shapes.h"

class CollisionComponentData
{
    public:
        Collision::FCollisionQueryResult prevColliders;
        int                              cell;
};

class SphereComponent : public Component<SphereComponent>, public CollisionComponentData
{
    public:
        Shapes::FSphere sphere;
};

class AABBComponent : public Component<AABBComponent>, public CollisionComponentData
{
    public:
        Shapes::FAabb aabb;
};

class CapsuleComponent : public Component<CapsuleComponent>, public CollisionComponentData
{
    public:
        Shapes::FCapsule capsule;
};

class PlaneComponent : public Component<PlaneComponent>, public CollisionComponentData
{
    public:
        Shapes::FPlane plane;
};