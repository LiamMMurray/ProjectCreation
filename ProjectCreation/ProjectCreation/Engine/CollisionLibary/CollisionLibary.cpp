#include "CollisionLibary.h"
#include "../MathLibrary/MathLibrary.h"
using namespace DirectX;

CollisionComponent::FPlane CollisionLibary::CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c)
{
        CollisionComponent::FPlane output;
        output.normal = XMVector3Normalize(XMVector3Cross((b - a), (c - a)));
        output.offset = XMVectorGetX(XMVector3Dot(output.normal, c));

        return output;
}

CollisionComponent::FCotactPoint CollisionLibary::CollidePoint(DirectX::XMVECTOR pointA,
                                                               DirectX::XMVECTOR pointB,
                                                               float             radius) // raduis must bne pointB's radius
{

        CollisionComponent::FCotactPoint output;

        XMVECTOR tempDir    = XMVector3Normalize(pointA - pointB);
        XMVECTOR contactPos = pointB + tempDir * radius;
        output.position     = contactPos;
        output.normal       = tempDir;

        return output;
}

CollisionComponent::FCollideResult CollisionLibary::OverlapSphereToSphere(CollisionComponent::FSphere& a,
                                                                          CollisionComponent::FSphere& b,
                                                                          float                        offset)
{
        CollisionComponent::FCollideResult output;
        CollisionComponent::FCotactPoint   contactPoint;
        float                              distance    = MathLibrary::CalulateDistance(a.center, b.center) + offset;
        float                              totalRadius = a.radius + b.radius;
        if (distance < totalRadius)
        {
                output.collisionType = CollisionComponent::ECollisionType::EOveralap;
        }
        else if (distance > totalRadius)
        {
                output.collisionType = CollisionComponent::ECollisionType::ENoCollision;
        }
        else if (distance == totalRadius)
        {
                output.collisionType = CollisionComponent::ECollisionType::ECollide;
                /*XMVECTOR tempDir      = XMVector3Normalize(a.center - b.center);
                XMVECTOR contactPos   = b.center + tempDir * b.radius;
                contactPoint.position = contactPos;
                contactPoint.normal   = tempDir;*/
                output.collideSurfaces.push_back(CollidePoint(a.center, b.center, b.radius));

                /*    XMVECTOR tempDirA     = XMVector3Normalize(b.center - a.center);
                    XMVECTOR contactPosA  = a.center + tempDir * a.radius;
                    contactPoint.position = contactPosA;
                    contactPoint.normal   = tempDirA;*/
                output.collideSurfaces.push_back(CollidePoint(b.center, a.center, a.radius));
        }


        return output;
}

CollisionComponent::FSweepCollision CollisionLibary::SweepSphereToSphere(CollisionComponent::FSphere& startA,
                                                                         CollisionComponent::FSphere& endA,
                                                                         CollisionComponent::FSphere& checkB,
                                                                         float                        offset)
{
        CollisionComponent::FSweepCollision output;
        CollisionComponent::FCotactPoint    contactPoint;
        CollisionComponent::FCapsule        capsule;

        capsule.startPoint = startA.center;
        capsule.endPoint   = endA.center;
        capsule.radius     = startA.radius;

        XMVECTOR cloestPoint = MathLibrary::GetClosestPointFromLine(capsule.startPoint, capsule.endPoint, checkB.center);
        float    totalRadius = capsule.radius + checkB.radius;
        float    distance    = MathLibrary::CalulateDistance(cloestPoint, checkB.center) + offset;

        XMVECTOR direction = XMVector3Normalize(endA.center - startA.center); // gives the direction from startA to endA

        if (distance == totalRadius)
        {
                output.collisionType = CollisionComponent::ECollisionType::ECollide;
                output.collideSurfaces.push_back(CollidePoint(checkB.center, cloestPoint, capsule.radius));
                output.collideSurfaces.push_back(CollidePoint(cloestPoint, checkB.center, checkB.radius));
                CollisionComponent::FCotactPoint temp;
                temp                 = CollidePoint(checkB.center, cloestPoint, capsule.radius);
                XMVECTOR tempPos     = temp.position + (direction * checkB.radius) + (direction * capsule.radius);
                output.finalPoaition = tempPos;
        }
        else if (distance < totalRadius)
        {
                output.collisionType = CollisionComponent::ECollisionType::EOveralap;
                output.collideSurfaces.push_back(CollidePoint(checkB.center, cloestPoint, capsule.radius));
                output.collideSurfaces.push_back(CollidePoint(cloestPoint, checkB.center, checkB.radius));
                CollisionComponent::FCotactPoint temp;
                temp             = CollidePoint(checkB.center, cloestPoint, capsule.radius);
                XMVECTOR tempPos = temp.position + (direction * checkB.radius) + (direction * capsule.radius) ;
                output.finalPoaition = tempPos;
        }
        else if (distance > totalRadius)
        {
                output.collisionType = CollisionComponent::ECollisionType::ENoCollision;
        }
        return output;
}

CollisionComponent::FSweepCollision CollisionLibary::MovingSphereToMovingSphere(CollisionComponent::FSphere& a,
                                                                                CollisionComponent::FSphere& b,
                                                                                DirectX::XMVECTOR            velocityA,
                                                                                DirectX::XMVECTOR            velocityB,
                                                                                float&                       time,
                                                                                float                        offset,
                                                                                float                        epsilon)
{
        CollisionComponent::FSweepCollision output;
        XMVECTOR                            sphereDir   = b.center - a.center;
        XMVECTOR                            velocity    = velocityB - velocityA;
        float                               TotalRadius = a.radius + b.radius;
        float                               c = MathLibrary::VectorDotProduct(sphereDir, sphereDir) - TotalRadius * TotalRadius;
        if (c < 0.0f)
        {
                time                 = 0.0f;
                output.collisionType = CollisionComponent::EOveralap;
        }

        float velocityDotValue = MathLibrary::VectorDotProduct(velocity, velocity);
        if (velocityDotValue < epsilon)
        {
                output.collisionType = CollisionComponent::ENoCollision;
        }

        float DotValue = MathLibrary::VectorDotProduct(velocity, sphereDir);
        if (DotValue >= 0.0f)
        {
                output.collisionType = CollisionComponent::ENoCollision;
        }

        float d = DotValue * DotValue - velocityDotValue * c;
        if (d < 0.0f)
        {
                output.collisionType = CollisionComponent::ENoCollision;
        }

        time = (-DotValue - sqrtf(d)) / velocityDotValue;
        return output;
}


CollisionComponent::FCollideResult CollisionLibary::SphereToAabb(CollisionComponent::FSphere& sphere,
                                                                 CollisionComponent::FAabb&   aabb,
                                                                 float                        offset)
{
        CollisionComponent::FCollideResult output;
        XMVECTOR                           aabbMin           = aabb.center - aabb.extents;
        XMVECTOR                           aabbMax           = aabb.center + aabb.extents;
        XMVECTOR                           cloestPointinAABB = XMVectorMin(XMVectorMax(sphere.center, aabbMin), aabbMax);
        float                              distance = MathLibrary::CalulateDistance(cloestPointinAABB, sphere.center) + offset;


        if (distance < sphere.radius)
        {
                output.collisionType = CollisionComponent::EOveralap;
        }
        else if (distance > sphere.radius)
        {
                output.collisionType = CollisionComponent::ENoCollision;
        }
        else
        {
                output.collisionType = CollisionComponent::ECollide;
                output.collideSurfaces.push_back(CollidePoint(cloestPointinAABB, sphere.center, sphere.radius));
                output.collideSurfaces.push_back(CollidePoint(sphere.center, cloestPointinAABB, (distance - sphere.radius)));
        }
        return output;
}

CollisionComponent::FCollideResult CollisionLibary::AabbToAabb(CollisionComponent::FAabb& a,
                                                               CollisionComponent::FAabb& b,
                                                               float                      offset)
{
        CollisionComponent::FCollideResult output;

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

        if (aMaxX < bMinX || aMinX > bMaxX)
                output.collisionType = CollisionComponent::ENoCollision;
        if (aMaxY < bMinY || aMinY > bMaxY)
                output.collisionType = CollisionComponent::ENoCollision;
        if (aMaxZ < bMinZ || aMinZ > bMaxZ)
                output.collisionType = CollisionComponent::ENoCollision;


        output.collisionType = CollisionComponent::EOveralap;
        return output;
}
