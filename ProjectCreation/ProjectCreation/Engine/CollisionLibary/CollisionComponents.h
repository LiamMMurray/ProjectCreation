#pragma once
#include "../../ECS/Component.h"
#include "Shapes.h"

#include <unordered_set>

class CollisionComponentData
{
    public:
        std::unordered_set<ComponentHandle> collisionComponents;
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