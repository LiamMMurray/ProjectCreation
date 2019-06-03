#pragma once
#include "CollisionComponent.h"

class CollisionLibary
{
        static Shapes::FPlane CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c);

        static Collision::FContactPoint CalculateSphereToSphereContactPoint(DirectX::XMVECTOR pointA,
                                                                            DirectX::XMVECTOR pointB,
                                                                            float             radius);

    public:
        static void CreateFrustum(Shapes::Frustum& frustum, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
        static Collision::FCollideResult OverlapSphereToSphere(Shapes::FSphere& a, Shapes::FSphere& b, float offset);
        static Collision::FAdvancedviCollisionResult SweepSphereToSphere(Shapes::FSphere& startA,
                                                                         Shapes::FSphere& endA,
                                                                         Shapes::FSphere& checkB,
                                                                         float            offset);

        static Collision::FAdvancedviCollisionResult SphereSphereSweep(const Shapes::FSphere&  sphere1,
                                                                       const DirectX::XMVECTOR S1CurrPosition,
                                                                       const Shapes::FSphere&  sphere2,
                                                                       const DirectX::XMVECTOR S2CurrPosition,
                                                                       float& u0, // normalized time of first collision
                                                                       float& u1  // normalized time of second collision
        );
        static Collision::FAdvancedviCollisionResult MovingSphereToMovingSphere(Shapes::FSphere&  a,
                                                                                Shapes::FSphere&  b,
                                                                                DirectX::XMVECTOR velocityA,
                                                                                DirectX::XMVECTOR velocityB,
                                                                                float&            time,
                                                                                float             offset,
                                                                                float             epsilon);
        static Collision::FCollideResult OverlapSphereToAabb(Shapes::FSphere& sphere, Shapes::FAabb& aabb, float offset);
        static Collision::FCollideResult OverlapAabbToAabb(Shapes::FAabb& a, Shapes::FAabb& b, float offset);
      static Collision::FCollisionObjects CollisionQuery(Shapes::FSphere& startA, Shapes::FSphere& endA);

        static Collision::FCollisionObjects SweepAndPruneCollision(Shapes::FSphere& sphere, Shapes::FAabb& aabbSpace);

        static Collision::FAdvancedviCollisionResult RayToSphereCollision(DirectX::XMVECTOR& startPoint,
                                                                          DirectX::XMVECTOR& direction,
                                                                          Shapes::FSphere&   sphere);
        static Shapes::FAabb                         AddAABB(const Shapes::FAabb& a, const Shapes::FAabb& b);

        static Shapes::FAabb CreateBoundingBoxFromShpere(const Shapes::FSphere sphere);

        static Shapes::FAabb CreateBoundingBoxFromCapsule(const Shapes::FCapsule capsule);
        // gjk  narrow phase collision
        // bvh  broad phrase collision
		
};