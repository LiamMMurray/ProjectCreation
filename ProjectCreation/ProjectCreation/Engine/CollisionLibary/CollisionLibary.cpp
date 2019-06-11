#include "CollisionLibary.h"
#include <unordered_map>
#include "../MathLibrary/MathLibrary.h"
#include "CollisionGrid.h"
using namespace std;
using namespace DirectX;
using namespace Collision;
using namespace Shapes;
FPlane CollisionLibary::CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c)
{
        FPlane output;
        output.normal = XMVector3Normalize(XMVector3Cross((b - a), (c - a)));
        output.offset = XMVectorGetX(XMVector3Dot(output.normal, c));

        return output;
}

FContactPoint CollisionLibary::CalculateSphereToSphereContactPoint(DirectX::XMVECTOR pointA,
                                                                   DirectX::XMVECTOR pointB,
                                                                   float             radiusA)
{

        FContactPoint output;

        output.normal       = XMVector3Normalize(pointA - pointB);
        XMVECTOR contactPos = pointA - output.normal * radiusA;
        output.position     = contactPos;

        return output;
}

void CollisionLibary::CreateFrustum(Shapes::Frustum& frustum, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
{
        std::array<XMVECTOR, 8> points;


        points[0] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        points[1] = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
        points[2] = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
        points[3] = XMVectorSet(1.0f, 1.0f, 0.0f, 1.0f);

        points[4] = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
        points[5] = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
        points[6] = XMVectorSet(0.0f, 1.0f, 1.0f, 1.0f);
        points[7] = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

        for (int i = 0; i < points.size(); ++i)
        {
                points[i] = XMVector3Unproject(points[i], 0, 0, 1, 1, 0, 1, projection, view, XMMatrixIdentity());
        }

        // fornt face
        frustum[0] = CalculatePlane(points[0], points[2], points[3]);
        // back face
        frustum[1] = CalculatePlane(points[5], points[7], points[6]);
        // right face
        frustum[2] = CalculatePlane(points[5], points[1], points[3]);
        // left face
        frustum[3] = CalculatePlane(points[4], points[6], points[2]);
        // up face
        frustum[4] = CalculatePlane(points[4], points[0], points[1]);
        // down face
        frustum[5] = CalculatePlane(points[6], points[7], points[3]);
}

FOverlapResult CollisionLibary::OverlapSphereToSphere(FSphere& a, FSphere& b, float offset)
{
        FOverlapResult output;
        FContactPoint  contactPoint;
        float          distance    = MathLibrary::CalulateDistance(a.center, b.center) + offset;
        float          totalRadius = a.radius + b.radius;
        if (distance <= totalRadius)
        {
                // output.collisionType = ECollisionType::EOveralap;
                output.hasOverlap = true;
        }
        else
        {
                // output.collisionType = ECollisionType::ENoCollision;
                output.hasOverlap = false;
        }

        return output;
}

FAdvancedCollisionResult CollisionLibary::SweepSphereToSphere(FSphere& startA, FSphere& endA, FSphere& checkB, float offset)
{
        FAdvancedCollisionResult output;
        FContactPoint            contactPoint;
        FCapsule                 capsule;

        capsule.startPoint = startA.center;
        capsule.endPoint   = endA.center;
        capsule.radius     = startA.radius;

        XMVECTOR cloestPoint = MathLibrary::GetClosestPointFromLine(capsule.startPoint, capsule.endPoint, checkB.center);

        FSphere closestSphere;
        closestSphere.center = cloestPoint;
        closestSphere.radius = capsule.radius;

        float totalRadius = capsule.radius + checkB.radius;
        float distance    = MathLibrary::CalulateDistance(closestSphere.center, checkB.center);

        XMVECTOR direction = XMVector3Normalize(endA.center - startA.center); // gives the direction from startA to endA
        if (distance < totalRadius)
        {
                output.collisionType = ECollisionType::ECollide;

                output.finalPosition = closestSphere.center - direction * (totalRadius - distance); //+ offset);

                auto ClosestContactPoint =
                    CalculateSphereToSphereContactPoint(output.finalPosition, checkB.center, closestSphere.radius);

                output.finalDirection = ClosestContactPoint.normal;
        }
        else if (distance > totalRadius)
        {
                output.collisionType = ECollisionType::ENoCollision;
        }
        return output;
}

Collision::FAdvancedCollisionResult CollisionLibary::SphereSphereSweep(const Shapes::FSphere&  sphere1,
                                                                       const DirectX::XMVECTOR S1CurrPosition,
                                                                       const Shapes::FSphere&  sphere2,
                                                                       const DirectX::XMVECTOR S2CurrPosition,
                                                                       float& u0, // normalized time of first collision
                                                                       float& u1) // normalized time of second collision
{
        FAdvancedCollisionResult output;

        const XMVECTOR va = S1CurrPosition - sphere1.center; // A1 - A0;

        // vector from A0 to A1

        const XMVECTOR vb = S2CurrPosition - sphere2.center; // B1 - B0;
        // vector from B0 to B1

        const XMVECTOR AB = sphere2.center - sphere1.center; // B0 - A0;
        // vector from A0 to B0

        const XMVECTOR vab = vb - va;
        // relative velocity (in normalized time)

        const float rab = sphere1.radius + sphere2.radius;

        const float a = MathLibrary::VectorDotProduct(vab, vab);

        // u*u coefficient

        const float b = 2 * MathLibrary::VectorDotProduct(vab, AB);
        // u coefficient

        const float c = MathLibrary::VectorDotProduct(AB, AB) - rab * rab;
        // constant term
        // check if they're currently overlapping
        if (MathLibrary::VectorDotProduct(AB, AB) <= rab * rab)

        {
                u0                   = 0;
                u1                   = 0;
                output.collisionType = ECollide;

                output.finalPosition  = sphere1.center;
                output.finalDirection = XMVector3Normalize(sphere1.center - sphere2.center);
                return output;
        }

        // check if they hit each other
        // during the frame
        float totalDist = MathLibrary::CalulateVectorLength(va) + MathLibrary::CalulateVectorLength(vb);
        if (totalDist > 0 && MathLibrary::QuadraticFormula(a, b, c, u0, u1))
        {
                if ((u0 >= 0 && u0 <= 1) || (u1 >= 0 && u1 <= 1))
                {
                        if (u0 > u1)
                                swap(u0, u1);
                        output.collisionType = ECollide;
                        output.finalPosition = XMVectorLerp(sphere1.center, S1CurrPosition, u0);
                        output.finalDirection =
                            XMVector3Normalize(output.finalPosition - XMVectorLerp(sphere2.center, S2CurrPosition, u0));
                        return output;
                }
        }

        output.collisionType = ENoCollision;
        return output;
}

FAdvancedCollisionResult CollisionLibary::MovingSphereToMovingSphere(FSphere&          a,
                                                                     FSphere&          b,
                                                                     DirectX::XMVECTOR velocityA,
                                                                     DirectX::XMVECTOR velocityB,
                                                                     float&            time,
                                                                     float             offset,
                                                                     float             epsilon)
{
        FAdvancedCollisionResult output;
        XMVECTOR                 sphereDir   = b.center - a.center;
        XMVECTOR                 velocity    = velocityB - velocityA;
        float                    TotalRadius = a.radius + b.radius;
        float                    c           = MathLibrary::VectorDotProduct(sphereDir, sphereDir) - TotalRadius * TotalRadius;
        if (c < 0.0f)
        {
                time                 = 0.0f;
                output.collisionType = EOveralap;

                return output;
        }

        float velocityDotValue = MathLibrary::VectorDotProduct(velocity, velocity);
        if (velocityDotValue < epsilon)
        {
                output.collisionType = ENoCollision;
        }

        float DotValue = MathLibrary::VectorDotProduct(velocity, sphereDir);
        if (DotValue >= 0.0f)
        {
                output.collisionType = ENoCollision;
        }

        float d = DotValue * DotValue - velocityDotValue * c;
        if (d < 0.0f)
        {
                output.collisionType = ENoCollision;
        }

        time                = (-DotValue - sqrtf(d)) / velocityDotValue;
        XMVECTOR direction1 = XMVector3Normalize(a.center - b.center);
        XMVECTOR direction2 = XMVector3Normalize(b.center - a.center);

        FContactPoint temp;
        temp             = CalculateSphereToSphereContactPoint(a.center, b.center, b.radius);
        XMVECTOR tempPos = temp.position + (direction2 * b.radius);
        b.center         = tempPos * time;


        temp              = CalculateSphereToSphereContactPoint(b.center, a.center, a.radius);
        XMVECTOR tempPos1 = temp.position + (direction1 * a.radius);
        a.center          = tempPos1 * time;


        output.collisionType = ECollide;
        return output;
}


FOverlapResult CollisionLibary::OverlapSphereToAabb(FSphere& sphere, FAabb& aabb, float offset)
{
        FOverlapResult output;
        XMVECTOR       aabbMin           = aabb.center - aabb.extents;
        XMVECTOR       aabbMax           = aabb.center + aabb.extents;
        XMVECTOR       cloestPointinAABB = XMVectorMin(XMVectorMax(sphere.center, aabbMin), aabbMax);
        float          distance          = MathLibrary::CalulateDistance(cloestPointinAABB, sphere.center) + offset;

        XMVECTOR direction = XMVector3Normalize(aabb.center - sphere.center);
        if (distance <= sphere.radius)
        {
                // output.collisionType = EOveralap;
                output.hasOverlap = true;
        }
        else if (distance > sphere.radius)
        {
                //  output.collisionType = ENoCollision;
                output.hasOverlap = false;
        }

        return output;
}

FOverlapResult CollisionLibary::OverlapAabbToAabb(FAabb& a, FAabb& b, float offset)
{
        FOverlapResult output;

        XMVECTOR aMax = a.center + a.extents;
        XMVECTOR aMin = a.center - a.extents;
        XMVECTOR bMax = b.center + b.extents;
        XMVECTOR bMin = b.center - b.extents;

        float aMaxX = XMVectorGetX(aMax);
        float aMaxY = XMVectorGetY(aMax);
        float aMaxZ = XMVectorGetZ(aMax);

        float aMinX = XMVectorGetX(aMin);
        float aMinY = XMVectorGetY(aMin);
        float aMinZ = XMVectorGetZ(aMin);

        float bMaxX = XMVectorGetX(bMax);
        float bMaxY = XMVectorGetY(bMax);
        float bMaxZ = XMVectorGetZ(bMax);

        float bMinX = XMVectorGetX(bMin);
        float bMinY = XMVectorGetY(bMin);
        float bMinZ = XMVectorGetZ(bMin);

        output.hasOverlap = false;
        if (aMaxX < bMinX || aMinX > bMaxX || aMaxY < bMinY || aMinY > bMaxY || aMaxZ < bMinZ || aMinZ > bMaxZ)
                return output;

        output.hasOverlap = true;
        return output;
}

Collision::FAdvancedCollisionResult CollisionLibary::RayToSphereCollision(DirectX::XMVECTOR& startPoint,
                                                                          DirectX::XMVECTOR& directoin,
                                                                          Shapes::FSphere&   sphere)
{
        FAdvancedCollisionResult output;
        XMVECTOR                 vectorToTarget = startPoint - sphere.center;
        float                    dot            = MathLibrary::VectorDotProduct(directoin, vectorToTarget);
        XMVECTOR                 length         = directoin * dot;
        XMVECTOR                 cloestPoint    = startPoint + length;

        float distance    = MathLibrary::VectorDotProduct((sphere.center - cloestPoint), (sphere.center - cloestPoint));
        float totalRadius = sphere.radius * sphere.radius;
        if (distance <= totalRadius)
        {
                output.collisionType  = ECollide;
                output.finalDirection = directoin;
                output.finalPosition  = cloestPoint;
        }
        else if (distance > totalRadius)
        {
                output.collisionType = ENoCollision;
        }

        return output;
}

Collision::FCollideResult CollisionLibary::CircleToCircleCollision(Shapes::FCircle& a, Shapes::FCircle& b)
{
        Collision::FCollideResult output;
        float                     totalRadius = (a.radius + b.radius) * (a.radius + b.radius);
        float                     distance =
            ((a.center.x - b.center.x) * (a.center.x - b.center.x)) + ((a.center.y - b.center.y) * (a.center.y - b.center.y));
        if (distance <= totalRadius)
        {
                output.collisionType = Collision::ECollide;
        }
        else
        {
                output.collisionType = Collision::ENoCollision;
        }
        return output;
}

FAabb CollisionLibary::AddAABB(const FAabb& a, const FAabb& b)
{
        FAabb    aabb;
        XMVECTOR min = XMVectorMin(a.center - a.extents, b.center - b.extents);
        XMVECTOR max = XMVectorMax(a.center + a.extents, b.center + b.extents);

        aabb.center  = (min + max) / 2.0f;
        aabb.extents = max - aabb.center;

        return aabb;
}

Shapes::FAabb CollisionLibary::CreateBoundingBoxFromShpere(const Shapes::FSphere sphere)
{
        FAabb output;
        output.center = XMVectorSet(XMVectorGetX(sphere.center) + sphere.radius,
                                    XMVectorGetY(sphere.center) + sphere.radius,
                                    XMVectorGetZ(sphere.center) + sphere.radius,
                                    0.0f);
        output.extents =
            XMVectorSet(sphere.radius + sphere.radius, sphere.radius + sphere.radius, sphere.radius + sphere.radius, 0.0f);
        return output;
}

Shapes::FAabb CollisionLibary::CreateBoundingBoxFromCapsule(const Shapes::FCapsule capsule)
{
        FAabb output;
        output.center  = MathLibrary::GetMidPointFromTwoVector(capsule.startPoint, capsule.endPoint);
        float length   = MathLibrary::CalulateVectorLength(output.center - capsule.startPoint);
        output.extents = XMVectorSet(capsule.radius + length, capsule.radius + length, capsule.radius + length, 0.0f);
        return output;
}

Collision::FOverlapResult CollisionLibary::ScreenSpaceOverlap(const Shapes::FSphere&   a,
                                                              const Shapes::FSphere&   b,
                                                              const DirectX::XMMATRIX& ViewProjection)
{
        Collision::FOverlapResult output;
        XMVECTOR                  center1 = XMVector4Transform(a.center, ViewProjection);
        XMVECTOR                  center2 = XMVector4Transform(b.center, ViewProjection);
        float                     w1      = XMVectorGetW(center1);
        float                     w2      = XMVectorGetW(center2);
        center1                           = center1 / w1; // get the screen space vector of sphere
        center2                           = center2 / w2;

        float radius1 = a.radius / w1;
        float radius2 = b.radius / w2;

        FSphere screenSphere1(center1, radius1);
        FSphere screenSphere2(center2, radius2);
        output = OverlapSphereToSphere(screenSphere1, screenSphere2, 0.0f);

        return output;
}
