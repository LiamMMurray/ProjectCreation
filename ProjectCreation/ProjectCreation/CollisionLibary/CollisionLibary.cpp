#include "CollisionLibary.h"
using namespace DirectX;

CollisionComponent::FCollideResult CollisionLibary::OverlapSphereToSphere(CollisionComponent::FSphere& a,
                                                                          CollisionComponent::FSphere& b)
{
        CollisionComponent::FCollideResult output;
        CollisionComponent::FCotactPoint   contactPoint;
        float                              distance    = XMVectorGetX(XMVector3Dot(a.center, b.center));
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
                output.collisionType  = CollisionComponent::ECollisionType::ECollide;
                XMVECTOR tempDir      = XMVector3Normalize(a.center - b.center);
                XMVECTOR contactPos   = b.center + tempDir * b.radius;
                contactPoint.position = contactPos;
                contactPoint.normal   = tempDir;
                output.collideSurfaces.push_back(contactPoint);

                XMVECTOR tempDirA      = XMVector3Normalize(b.center - a.center);
                XMVECTOR contactPosA   = a.center + tempDir * a.radius;
                contactPoint.position = contactPosA;
                contactPoint.normal   = tempDirA;
                output.collideSurfaces.push_back(contactPoint);
        }


        return output;
}

CollisionComponent::FCollideResult CollisionLibary::SweepSphereToSphere(CollisionComponent::FSphere& startA,
                                                                        CollisionComponent::FSphere& endA,
                                                                        CollisionComponent::FSphere& checkB)
{
        CollisionComponent::FCollideResult output;
        CollisionComponent::FCapsule       capsule;
        XMVECTOR                           cloestPoint;
        capsule.startPoint = startA.center;
        capsule.endPoint   = endA.center;
        capsule.radius = startA.radius;

		float totalRadius = capsule.radius + checkB.radius;
		
        return output;
}
