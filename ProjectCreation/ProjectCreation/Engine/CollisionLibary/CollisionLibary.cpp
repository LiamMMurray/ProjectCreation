#include "CollisionLibary.h"
#include "../MathLibrary/MathLibrary.h"
using namespace DirectX;
using namespace CollisionComponent;
using namespace Shapes;
FPlane CollisionLibary::CalculatePlane(DirectX::XMVECTOR a, DirectX::XMVECTOR b, DirectX::XMVECTOR c)
{
        FPlane output;
        output.normal = XMVector3Normalize(XMVector3Cross((b - a), (c - a)));
        output.offset = XMVectorGetX(XMVector3Dot(output.normal, c));

        return output;
}

FCotactPoint CollisionLibary::CollidePoint(DirectX::XMVECTOR pointA,
                                           DirectX::XMVECTOR pointB,
                                           float             radius) // raduis must bne pointB's radius
{

        FCotactPoint output;

        XMVECTOR tempDir    = XMVector3Normalize(pointA - pointB);
        XMVECTOR contactPos = pointB + tempDir * radius;
        output.position     = contactPos;
        output.normal       = tempDir;

        return output;
}

FCollideResult CollisionLibary::OverlapSphereToSphere(FSphere& a, FSphere& b, float offset)
{
        FCollideResult output;
        FCotactPoint   contactPoint;
        float          distance    = MathLibrary::CalulateDistance(a.center, b.center) + offset;
        float          totalRadius = a.radius + b.radius;
        if (distance < totalRadius)
        {
                output.collisionType = ECollisionType::EOveralap;
        }
        else if (distance > totalRadius)
        {
                output.collisionType = ECollisionType::ENoCollision;
        }
        else if (distance == totalRadius)
        {
                output.collisionType = ECollisionType::ECollide;
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

FSweepCollisionResult CollisionLibary::SweepSphereToSphere(FSphere& startA, FSphere& endA, FSphere& checkB, float offset)
{
        FSweepCollisionResult output;
        FCotactPoint          contactPoint;
        FCapsule              capsule;

        capsule.startPoint = startA.center;
        capsule.endPoint   = endA.center;
        capsule.radius     = startA.radius;

        XMVECTOR cloestPoint = MathLibrary::GetClosestPointFromLine(capsule.startPoint, capsule.endPoint, checkB.center);

		FSphere tempSphere;
        tempSphere.center = cloestPoint;
        tempSphere.radius = capsule.radius;

        float    totalRadius = capsule.radius + checkB.radius;
        float distance    = MathLibrary::CalulateDistance(tempSphere.center, checkB.center);

        XMVECTOR direction = XMVector3Normalize(endA.center - startA.center); // gives the direction from startA to endA

        if (distance <= totalRadius)
        {
                output.collisionType = ECollisionType::ECollide;
                output.collideSurfaces.push_back(CollidePoint(checkB.center, cloestPoint, capsule.radius));
                output.collideSurfaces.push_back(CollidePoint(cloestPoint, checkB.center, checkB.radius));

                output.finalPosition = tempSphere.center - direction*(totalRadius - distance + offset);

        }
        else if (distance > totalRadius)
        {
                output.collisionType = ECollisionType::ENoCollision;
        }
        return output;
}

FSweepCollisionResult CollisionLibary::MovingSphereToMovingSphere(FSphere&          a,
                                                                  FSphere&          b,
                                                                  DirectX::XMVECTOR velocityA,
                                                                  DirectX::XMVECTOR velocityB,
                                                                   float&            time,
                                                                  float             offset,
                                                                  float             epsilon)
{
        FSweepCollisionResult output;
        XMVECTOR              sphereDir   = b.center - a.center;
        XMVECTOR              velocity    = velocityB - velocityA;
        float                 TotalRadius = a.radius + b.radius;
        float                 c           = MathLibrary::VectorDotProduct(sphereDir, sphereDir) - TotalRadius * TotalRadius;
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

        time = (-DotValue - sqrtf(d)) / velocityDotValue;
        XMVECTOR direction1 = XMVector3Normalize(a.center - b.center);
        XMVECTOR direction2 = XMVector3Normalize(b.center - a.center); 
        
		FCotactPoint temp;
        temp                 = CollidePoint(a.center, b.center, b.radius);
        XMVECTOR tempPos     = temp.position + (direction2 * b.radius);
        b.center             = tempPos * time;


		temp             = CollidePoint(b.center, a.center, a.radius);
        XMVECTOR tempPos1 = temp.position + (direction1 * a.radius);
        a.center         = tempPos1 * time;



        output.collisionType = ECollide;
        return output;
}


FCollideResult CollisionLibary::SphereToAabb(FSphere& sphere, FAabb& aabb, float offset)
{
        FCollideResult output;
        XMVECTOR       aabbMin           = aabb.center - aabb.extents;
        XMVECTOR       aabbMax           = aabb.center + aabb.extents;
        XMVECTOR       cloestPointinAABB = XMVectorMin(XMVectorMax(sphere.center, aabbMin), aabbMax);
        float          distance          = MathLibrary::CalulateDistance(cloestPointinAABB, sphere.center) + offset;

		XMVECTOR direction = XMVector3Normalize(aabb.center - sphere.center); 
        if (distance < sphere.radius)
        {
                output.collisionType = EOveralap;
        }
        else if (distance > sphere.radius)
        {
                output.collisionType = ENoCollision;
        }
        else
        {
                output.collisionType = ECollide;
                output.collideSurfaces.push_back(CollidePoint(cloestPointinAABB, sphere.center, sphere.radius));
                output.collideSurfaces.push_back(CollidePoint(sphere.center, cloestPointinAABB, (distance - sphere.radius)));
                FCotactPoint temp;
                temp                 = CollidePoint(aabb.center, sphere.center, sphere.radius);
                XMVECTOR tempPos     = temp.position + (direction * sphere.radius) + (direction * (aabb.center + aabb.extents));
               // output.finalPoaition = tempPos;
                sphere.center = tempPos;
        }
        return output;
}

FCollideResult CollisionLibary::AabbToAabb(FAabb& a, FAabb& b, float offset)
{
        FCollideResult output;

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
                output.collisionType = ENoCollision;
        if (aMaxY < bMinY || aMinY > bMaxY)
                output.collisionType = ENoCollision;
        if (aMaxZ < bMinZ || aMinZ > bMaxZ)
                output.collisionType = ENoCollision;


        output.collisionType = EOveralap;
        return output;
}

FCollisionObjects CollisionLibary::CollisionQuery(FSphere& startA, FSphere& endA)
{
        FCollisionObjects output;
        FCapsule          capsule;


        return output;
}

FCollisionObjects CollisionLibary::SweepAndPruneCollision(FSphere& sphere, FAabb& aabbSpace)
{

        return FCollisionObjects();
}

FAabb CollisionLibary::AdddAABB(const FAabb& a, const FAabb& b)
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
                                    XMVectorGetZ                                                                                      (sphere.center) + sphere.radius,
                                    0.0f);
        output.extents = 
            XMVectorSet(sphere.radius + sphere.radius, sphere.radius + sphere.radius, sphere.radius + sphere.radius, 0.0f);
        return output;
}
