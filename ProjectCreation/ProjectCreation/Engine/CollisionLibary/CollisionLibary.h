#pragma once
#include"CollisionResult.h"

class CollisionLibary
{
        static Shapes::FPlane CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c);

        static Collision::FContactPoint CalculateSphereToSphereContactPoint(DirectX::XMVECTOR pointA,
                                                                            DirectX::XMVECTOR pointB,
                                                                            float             radius);

    public:
        static void CreateFrustum(Shapes::Frustum& frustum, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);
        static Collision::FCollideResult OverlapSphereToSphere(Shapes::FSphere& a, Shapes::FSphere& b, float offset);
        static Collision::FAdvancedCollisionResult SweepSphereToSphere(Shapes::FSphere& startA,
                                                                         Shapes::FSphere& endA,
                                                                         Shapes::FSphere& checkB,
                                                                         float            offset);

        static Collision::FAdvancedCollisionResult SphereSphereSweep(const Shapes::FSphere&  sphere1,
                                                                       const DirectX::XMVECTOR S1CurrPosition,
                                                                       const Shapes::FSphere&  sphere2,
                                                                       const DirectX::XMVECTOR S2CurrPosition,
                                                                       float& u0, // normalized time of first collision
                                                                       float& u1  // normalized time of second collision
        );
        static Collision::FAdvancedCollisionResult MovingSphereToMovingSphere(Shapes::FSphere&  a,
                                                                                Shapes::FSphere&  b,
                                                                                DirectX::XMVECTOR velocityA,
                                                                                DirectX::XMVECTOR velocityB,
                                                                                float&            time,
                                                                                float             offset,
                                                                                float             epsilon);
        static Collision::FCollideResult OverlapSphereToAabb(Shapes::FSphere& sphere, Shapes::FAabb& aabb, float offset);
        static Collision::FCollideResult OverlapAabbToAabb(Shapes::FAabb& a, Shapes::FAabb& b, float offset);

		static std::pair<Collision::FCollideResult, Shapes::FCollisionShape*> CollisionQueries(Shapes::FCollisionShape* shape);

        static Collision::FAdvancedCollisionResult RayToSphereCollision(DirectX::XMVECTOR& startPoint,
                                                                          DirectX::XMVECTOR& direction,
                                                                          Shapes::FSphere&   sphere);

		static Collision::FCollideResult CircleToCircleCollision(Shapes::FCircle& a, Shapes::FCircle& b);
        static Shapes::FAabb                         AddAABB(const Shapes::FAabb& a, const Shapes::FAabb& b);

        static Shapes::FAabb CreateBoundingBoxFromShpere(const Shapes::FSphere sphere);

        static Shapes::FAabb CreateBoundingBoxFromCapsule(const Shapes::FCapsule capsule);

		
};