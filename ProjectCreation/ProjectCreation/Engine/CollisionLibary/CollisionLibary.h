#pragma once
#include "CollisionComponent.h"

class CollisionLibary
{
        Shapes::FPlane              CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c);

        static CollisionComponent::FCotactPoint CollidePoint(DirectX::XMVECTOR pointA, DirectX::XMVECTOR pointB, float radius);

    public:
        static CollisionComponent::FCollideResult    OverlapSphereToSphere(Shapes::FSphere&             a, Shapes::FSphere& b,
                                                                           float                        offset);
        static CollisionComponent::FSweepCollisionResult SweepSphereToSphere(Shapes::FSphere& startA,
                                                                         Shapes::FSphere&             endA,
                                                                         Shapes::FSphere& checkB,
                                                                         float                        offset);
        static CollisionComponent::FSweepCollisionResult MovingSphereToMovingSphere(Shapes::FSphere&  a,
                                                                                Shapes::FSphere&  b,
                                                                                DirectX::XMVECTOR            velocityA,
                                                                                DirectX::XMVECTOR            velocityB,
                                                                                float&                       time,
                                                                                float                        offset,
                                                                                float                        epsilon);
        static CollisionComponent::FCollideResult    SphereToAabb(Shapes::FSphere&           sphere, Shapes::FAabb& aabb,
                                                                  float                        offset);
        static CollisionComponent::FCollideResult    AabbToAabb(Shapes::FAabb& a, Shapes::FAabb& b,
                                                                float                      offset);
        static CollisionComponent::FCollisionObjects CollisionQuery(Shapes::FSphere& startA, Shapes::FSphere& endA);

        static CollisionComponent::FCollisionObjects SweepAndPruneCollision(Shapes::FSphere&           sphere, Shapes::FAabb& aabbSpace);

        static CollisionComponent::FCollisionObjects BroadPhaseCollision();

        static Shapes::FAabb AdddAABB(const Shapes::FAabb& a, const Shapes::FAabb& b);

		static Shapes::FAabb CreateBoundingBoxFromShpere(const Shapes::FSphere sphere);

        // gjk  narrow phase collision
        // bvh  broad phrase collision
};