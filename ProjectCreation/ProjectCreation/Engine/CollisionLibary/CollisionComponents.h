#pragma once
#include "../../ECS/Component.h"
#include "Shapes.h"

class SphereComponent : public Component<SphereComponent>
{
    public:
        Shapes::FSphere sphere;
};

class AABBComponent : public Component<AABBComponent>
{
    public:
        Shapes::FAabb aabb;
};

class CapsuleComponent : public Component<CapsuleComponent>
{
    public:
        Shapes::FCapsule capsule;
};

class PlaneComponent : public Component<PlaneComponent>
{
    public:
        Shapes::FPlane plane;
};